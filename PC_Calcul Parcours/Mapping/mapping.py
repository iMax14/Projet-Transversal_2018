# -*- coding: utf-8 -*-
"""
Created on Tue May 15 14:04:53 2018

@author: bot_ redefine the objects imported
"""

#IMPORT MODULES
import json
import serial
import cv2
import math
from skimage.transform import hough_line, hough_line_peaks
from skimage.draw import line
import matplotlib.pyplot as plt
import PIL as p
import Tkinter as tk
import numpy as np
from heapq import *
np.set_printoptions(threshold=np.nan)
from collections import namedtuple
from pprint import pprint
from time import sleep


#CREATION FENETRE
root = tk.Tk()
#taille matrice
H = 600;W = 600;

centre = 75#décalage fenetre

#Contient chemin calcule
PATH_X = []
PATH_Y = []
PATH =[]
#point de passage
pdp = []  
#ETAT LAMPE
lampe = False 
#STRING LIST D'ordre envoye au robot par serie
order = []
#coordonne d'arrivee[x][y][angle]
dest = []

#VARIABLE LABEL
action_encours = tk.StringVar()
action_encours.set("Pas d'action en cours")

varenergie = tk.StringVar()
varenergie.set("Energie consommée: ")

posRobot= tk.StringVar()
posRobot.set("Position ROBOT: X:Y:")

#'documentp3.json'

#LECTURE JSON
with open('document.json') as data_file:    
    terrain = json.load(data_file)
    
with open('document2.json') as data_file:    
    data = json.load(data_file)

#MATRICE REPRESENTANT LE TERRAIN (0 = obstacle; 1 = passage )
Map = np.ones((H,W), dtype=int);

#
#distance(ne sert à rien pour le moment)
distance = []

#ZONE DE DEPLACMEMENT DU RObOT(ne sert à rien pour le moment)
Zone = []
#position en x et y des vertices du terrain jouable
x = []
y = []
#position en x et y des vertices de la zone de cible
x_cible = []
y_cible = []

#Position X Y et Rayon des Obstacles Cercles
obstaclex = []
obstacley = []
rayon = []

#position en x et y des vertices des polygones Obstacle
obstacle_polX = []
obstacle_polY = []
#Nombre vertice des obstacles polygonial
taille_Pol = []


##############################################################
#####CHAREMENT DU FICHIER JSON DANS LES LISTES COREESPONDANTES
for i in range(len( terrain['terrain_evolution'] )  ):
    x.append(terrain['terrain_evolution'][i]['point']['x']+centre)
    y.append(terrain['terrain_evolution'][i]['point']['y']+centre)
    Zone.append((x,y))
    
for i in range(len( terrain['contour_cible'] )  ):
    x_cible.append(terrain['contour_cible'][i]['point']['x']+centre)
    y_cible.append(terrain['contour_cible'][i]['point']['y']+centre)
   

for i in range(len( terrain['obstacles']  ) ):
    #Map[[terrain['obstacles'][i]['coordonnees'][0]['centre']['x']],[terrain['obstacles'][i]['coordonnees'][0]['centre']['y']]] = 0
    #si obstacle est un cercle    
    if(terrain['obstacles'][i]['type'] == 'cercle'):
        obstaclex.append((terrain['obstacles'][i]['coordonnees'][0]['centre']['x']+centre))
        obstacley.append(terrain['obstacles'][i]['coordonnees'][0]['centre']['y']+centre)
        rayon.append(terrain['obstacles'][i]['coordonnees'][1]['rayon'])
    if(terrain['obstacles'][i]['type'] == 'polygone'):
        taille_Pol.append(len(terrain['obstacles'][i]['coordonnees']))
        for n in range(len(terrain['obstacles'][i]['coordonnees'])):
            obstacle_polX.append(terrain['obstacles'][i]['coordonnees'][n]['point']['x']+centre)
            obstacle_polY.append(terrain['obstacles'][i]['coordonnees'][n]['point']['y']+centre)




#######################################################################
######################################################################


#POSION DEPART DU ROBOT (start)
xs = data['depart']['coordonnees']['x'] + centre +25
ys = data['depart']['coordonnees']['y'] + centre +25
#POSION ARRIVEE DU ROBOT (end)
xe = data['arrivee']['coordonnees']['x'] + centre -25
ye = data['arrivee']['coordonnees']['y'] + centre -25
#POSION DE LA CIBLE
xc = terrain['cible']['centre']['x'] + centre
yc = terrain['cible']['centre']['y']+ centre
zc = terrain['cible']['hauteur']

#POINT DE PASSSAGE
etapeX = []
etapeY = []
etapeX.append(xs)
etapeY.append(ys)

#POSITION ROBOT [X][Y][ANGLE]
robot = []
robot.append(xs)
robot.append(ys)
robot.append(0)

posRobot.set("Position ROBOT: X:"+str(robot[0])+"Y:"+str(robot[1]))

for i in range(len(( data['etapes'] ))):
    etapeX.append(data['etapes'][i]['coordonnees']['x'] + centre)
    etapeY.append(data['etapes'][i]['coordonnees']['y'] + centre)

etapeX.append(xe)
etapeY.append(ye)


#CAPTEUR ULTRASON distance simple
def getDistance():
    order.append("MOU[D] :")

#CAPTEUR ULTRASON distance par balayage    
def Balayage(resolution):
    if resolution < 0 or resolution > 45 :
        resolution = 30
    order.append("MOB[D] A:"+normalisation2(resolution))


#POINTEUR LUMINEUX     
def allumage():
    global lampe
    if(lampe):
        order.append("L I:"+ normalisation3(50) +" D:"+normalisation3(50)+" E:"+ normalisation3(50)+" N:"+normalisation3(10) ) 
        lampe = False
    else:
        order.append("LS") # A DEFINIR
        lampe = True

#REMET ROBOT A ANGLE 0        
def raz_angle():
    D = robot[2] #angle du robot
    if(D <= 180):
        order.append("RA D:"+normalisation3(D)) #○tourne a droite  pour le remettre a angle 0
        robot[2] = 0
    else: #tourne a gauche
         order.append("RA G:"+normalisation3(D-180))#○tourne a gauche pour le remettre a angle 0
         robot[2] = 0

#INT TO STRING de taille 3    
def normalisation3(D):
    if(len(str(D))== 1):
        ret = "00"+str(D)
    elif(len(str(D))== 2):
        ret = "0"+str(D)
    else:
        ret = str(D)
    return ret

#INT TO STRING de taille 2
def normalisation2(D):
    if(len(str(D))== 1):
        ret = "0"+str(D)
    else:
        ret = str(D)
    return ret   
    
#GENERE la LISTE D'ordre POUR déplacement simple    
def ordre_gen():
    
    
    if(robot[2] != 0):
        raz_angle()
    #DEPLACEMENT X     
    D = abs(dest[0] - robot[0])
    if(dest[0] - robot[0] > 0):
        order.append("G X:"+str(D)+" Y:10 A:10")
    else: #tourne de 180
         order.append("RA G:"+str(180))#○tourne a gauche
         order.append("G X:"+str(D)+" Y:10 A:10")
    
    raz_angle()
    #DEPLACEMENT Y 
    if(dest[1] - robot[1] > 0):
        order.append("RA D:0"+str(90)) #○tourne a droite 
        robot[2] = robot[2] + 180 + 90
    else: #tourne a gauche        
         order.append("RA G:0"+str(90))#○tourne a gauche
         robot[2] = robot[2] + 90
    D = abs(dest[1] - robot[1])
    order.append("G X:"+str(D)+" Y:10 A:10")
    if(robot[2] != 0):
        raz_angle()
    D = dest[2]
    if D <= 180:
        order.append("RA G:"+normalisation3(D))#:gauche
    else:
        order.append("RA D:"+normalisation3(D-180))#:droite
        




#DEFINI UNE TRAJECTOIRE SIMPLE POUR LES COORD RENTREE A LA MAIN      
def trajectoire_simple():
    
    dx = dest[0] - robot[0]
    
    dy = dest[1] - robot[1]
    #theta = np.arctan(dy/dx)*180/np.pi
    r = np.sqrt(dx*dx + dy*dy)
    angle,distance = calcul_parcours()
    print(angle)
    
   
    L = c.create_line(robot[0] ,robot[1] ,robot[0] +dx ,robot[1] ,  fill="red")
    M = c.create_line(robot[0] +dx ,robot[1] ,robot[0] +dx ,robot[1]+dy ,  fill="red")
    
    N = c.create_line(robot[0] ,robot[1] ,robot[0] +distance*np.cos(angle)  ,robot[1]+distance* np.sin(angle) ,  fill="red")
    ordre_gen()    
    
#RECUPERE ORDRE (coord) rentre par utilisateur   
def getorder():
    dest.append(int(entrex.get())+centre)
    dest.append(int(entrey.get())+centre)
    dest.append(int(entrea.get()))
    
    entrex.delete(0, 'end')
    entrey.delete(0, 'end')
    entrea.delete(0, 'end')
    
    
    
    c.create_oval(dest[0]-10,dest[1]-10,dest[0]+10,dest[1]+10, outline="black", fill='yellow')
    c.create_line(dest[0],dest[1],dest[0]+10*np.cos(dest[2]*np.pi/180),dest[1]-10*np.sin(dest[2]*np.pi/180), fill='black')
    trajectoire_simple()
    robot[0] = dest[0]
    robot[1] = dest[1]
    robot[2] = dest[2]
    del dest[:]
    
    
        
#ENVOIE Des ordres par serie
def send_serie():
    out =[]
    #xbee = serial.Serial('/dev/ttyUSB0', baudrate=19200,timeout = 1.0)   
    #xbee = serial.Serial("COM4", baudrate=19200, timeout=1.0)
    #xbee.write("D\r")

    for i in range(len(order)):
       # xbee.write(order[i]+"\r")
       
        action_encours.set(("Action en cours: "+order[i]+"\r"))
        
        #xbee.write("MI :\r") #courant
        sleep(1)
        #while xbee.inWaiting() > 0:
         #   out += ser.read(1)
        print(out)
        #xbee.write("ME :\r") #energie
        sleep(1)
        #while xbee.inWaiting() > 0:
         #   out += ser.read(1)
        print(out)
        
        root.update_idletasks()
        print(order[i]+"\r")
        
    #xbee.write("E\r")    
    #xbee.close()
    action_encours.set("Pas d'action en cours")
    posRobot.set("Position ROBOT: X:"+str(robot[0]-centre)+"Y:"+str(robot[1]-centre))
    del order[:]

#ALGORITHME POUR Determiner si un point est dans le polygone (permet de fill le polygone décrit par ses sommets)
def point_in_poly(nvert, vertx, verty ,testx,testy):
    
    i = 0
    c = False 
    j = nvert -1
    for i in range(nvert):
        
        if(((verty[i]>testy) != (verty[j]>testy)) and (testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]) ):
            c = not c 
            
        j = i 
    
    return c

#REMPLI LA MATRICE MAP avec les obstacles et le terrain      
def createMat():   


# TERRAIN
    nvert = len(x)
    for i in range(min(x),max(x)):
        for j in range(min(y),max(y)):
            if( point_in_poly(nvert,x,y,i,j)):
                Map[j][i] = 0 #pas obstacle

    #obstacle cercle
    for n in range(len(obstaclex)):        
        for i in range(obstaclex[n]-rayon[n],obstaclex[n]+rayon[n]+1):
            for j in range(obstacley[n]-rayon[n],obstacley[n]+rayon[n]+1):
                #if( (i - obstaclex[n])*(i - obstaclex[n]) + (j - obstacley[n])*(j - obstacley[n]) < rayon[n]*rayon[n]):
                point_losangeX = [obstaclex[n]-rayon[n],obstaclex[n]+rayon[n],obstaclex[n]+rayon[n],obstaclex[n]-rayon[n]]
                point_losangeY = [obstacley[n]+rayon[n],obstacley[n]+rayon[n],obstacley[n]-rayon[n],obstacley[n]-rayon[n]]
                
                if(point_in_poly(4,point_losangeX, point_losangeY,i,j))  :              
                    Map[i][j] = 1 #obstacle
                    
                    
        
#obstacle Polygone
       
    indice = 0 
    for n in range(len(taille_Pol)): # NB de polygone    
        nvert = taille_Pol[n]
        for i in range(min(obstacle_polX[indice:indice + taille_Pol[n]]),max(obstacle_polX[indice:indice + taille_Pol[n]])+1): #Parcour de minX Pol à maxX Pol pour ne pas parcourrir toute la martice pour chaque poly
            for j in range(min(obstacle_polY[indice:indice + taille_Pol[n]]),max(obstacle_polY[indice:indice + taille_Pol[n]])+1):
                if(point_in_poly(nvert,obstacle_polX[indice:indice + taille_Pol[n]],obstacle_polY[indice:indice + taille_Pol[n]],i,j)):
                    Map[i][j] = 1 # obstacle
        indice = taille_Pol[n] + indice
  



def calcul_parcours():

    angle = float(0.0)

    if (robot[0] < dest[0]) and (robot[1] < dest[1]) :
        angle = np.fabs(np.arctan(np.fabs(dest[1] - robot[1])/np.fabs(dest[0]-robot[0])))

    if (robot[0] < dest[0]) and (robot[1] > dest[1]) :
        angle = (3*np.pi/2) + ((np.pi/2) - np.fabs(np.arctan(np.fabs(dest[1] - robot[1])/np.fabs(dest[0]-robot[0]))))


    if (robot[0] > dest[0]) and (robot[1] > dest[1]) :
        angle = (np.pi) + np.fabs(np.arctan(np.fabs(dest[1] - robot[1])/np.fabs(dest[0]-robot[0])))

    if (robot[0] > dest[0]) and (robot[1] < dest[1]) :
        angle = (np.pi/2) + ((np.pi/2) - np.fabs(np.arctan(np.fabs(dest[1] - robot[1])/np.fabs(dest[0]-robot[0]))))

    distance = np.sqrt(np.abs(dest[1] - robot[1])**2 + np.fabs(dest[0]-robot[0])**2)
	
    #angle = np.rad2deg(angle) 
    print(180*angle/np.pi)
    return angle,distance

def calcul_angles_pointeur():
    
    global xc,yc,zc #Coordonnées de la cible dans l'espace

    robot[2] = 10 #Hauteur à laquelle est placé le pointeur lumineux (en cm - unité à choisir) ### Hauteur à vérifier ###

    if (robot[1] <= yc) :
        Angle_servo_H = np.arctan(np.fabs(yc - robot[1])/np.fabs(xc - robot[0]))
	Angle_servo_H = 180*Angle_servo_H/np.pi #conversion en degré
    else :
        Angle_servo_H = -np.arctan(np.fabs(yc - robot[1])/np.fabs(xc - robot[0]))
	Angle_servo_H = 180*Angle_servo_H/np.pi #conversion en degré


    if (robot[0] <= xc) :
        Angle_servo_V = np.arctan(np.fabs(zc - robot[2])/np.sqrt(np.fabs(xc - robot[0])**2 + np.fabs(yc - robot[1])**2))
	Angle_servo_V = 180*Angle_servo_V/np.pi #conversion en degré
    else :
        Angle_servo_V = -np.arctan(np.fabs(zc - robot[2])/np.sqrt(np.fabs(xc - robot[0])**2 + np.fabs(yc - robot[1])**2))
	Angle_servo_V = 180*Angle_servo_V/np.pi #conversion en degré

   # Conversion des angles trouvés en instructions pour la carte MASTER

    Angle_servo_H = normalisation3(int(Angle_servo_H)) #retour de l'angle en char
    Angle_servo_V = normalisation3(int(Angle_servo_V)) #retour de l'angle en char

    order.append("CS H A:"+ str(Angle_servo_H)) 
    order.append("CS V A:"+ str(Angle_servo_V)) 
    allumage_Poiteur()
    
   

    

  
def find_vertice():

    
    #####################################"      MISE EN FORME DE LA TRAJEC POUR DES LIGNES DROITES
    listedecime = []
    listeangle = []
    virage = []
    dd = []
    #for i in range(0, len(PATH), 10):
    #	listedecime.append(PATH[i])
     
    listedecime = PATH
    
    for i in range(len(listedecime)-2):
    	ax = listedecime[i][0]
    	ay = listedecime[i][1]
    	bx = listedecime[i+1][0]
    	by = listedecime[i+1][1]
    	cx = listedecime[i+2][0]
    	cy = listedecime[i+2][1]
    	AB = np.sqrt((ax - bx)*(ax - bx)+ (ay - by)*(ay - by))
    	BC = np.sqrt((bx - cx)*(bx - cx) + (by - cy)*(by - cy))
    	CA = np.sqrt((cx - ax)*(cx - ax) + (cy - ay)*(cy - ay))
    	angle = math.acos((AB*AB + BC*BC - CA*CA) / (2 * AB * BC))
    	angle = angle/np.pi * 360
    	listeangle.append(angle)
    	
    	if (angle < 330):
    		virage.append(1)
    	else:
    		virage.append(0)
    	
    n = 0
    for i in range(len(virage)):
        if(virage[i] == 1):
            n=n+1
        else:
            if(n>1):
                dd.append(int(i-round(n/2)))
                n=0
            elif(n==1):
                dd.append(i)
                n=0
            else:
                n=0
             
         
		

    
    
    for i in dd:
        
        
        c.create_oval(PATH_X[i]-10,PATH_Y[i]-10,PATH_X[i]+10,PATH_Y[i]+10, outline="black", fill='green')                   
        print(PATH_X[i])
        print(PATH_Y[i])        
        pdp.append((PATH_X[i],PATH_Y[i]))       
    
    trajectoire_avance()
        

def trajectoire_avance():
    freq = 6
    duree_son =  25
    duree_sil = 50
    nbr_bip = 3
    dest.append(robot[0])
    dest.append(robot[1])
    dest.append(robot[2])    
    for i in range(len(pdp)):
        dest[0] = pdp[i][0]
        dest[1] = pdp[i][1]        
        [angle,distance] = calcul_parcours()
        robot[2] = angle
        robot[1] = dest[1]
        robot[0] = dest[0]
        
        if(angle < 180):
            order.append("RA D:"+normalisation3(int(round(int(angle)))))
        else:
            order.append("RA G:"+normalisation3(int(round(180-int(angle)))))
        
        order.append("G X:"+str(int(distance))+" Y:10 A:10")
        raz_angle()
        for j in range(len(etapeX)):
            if(abs(etapeX[j] - robot[0]) < 5 and abs(etapeY[j] - robot[1])<5):
                order.append("SD F:"+normalisation3(freq)+" P:"+normalisation3(duree_son)+" W:"+normalisation3(duree_sil)+" B:"+normalisation3(nbr_bip))
                
    for i in order:
            
            print(i+"\n")    
        
        
#REMPLI PATHX ET PATHY AVEC LE CHEMIN RETOURNE PAR ASTAR    
def chemin():
    createMat()
    
    

    for etape in range(len(etapeX)-1):
        path = astar(Map, (etapeX[etape], etapeY[etape]), (etapeX[etape+1],etapeY[etape+1]))
        for i in range(len(path)):
            PATH_X.append(path[i][0])
            PATH_Y.append(path[i][1])
            PATH.append((path[i][0],path[i][1]))
        #affichage
        for i in range(len(path)-1):
            c.create_line(path[i][0], path[i][1], path[i+1][0], path[i+1][1], fill="red")
        
    #print(PATH)
    find_vertice()
        
        
#plt.polar([0.0*3/180.0,10.0*3/180.0,20.0*3/180.0,30.0*3/180.0,50.0*3/180.0,60.0*3/180.0,70.0*3/180.0,80.0*3/180.0,90.0*3/180.0,100.0*3/180.0,110.0*3/180.0],)   



def graphdist():
    fig = plt.figure(figsize=(8,8))
    ax = fig.add_axes([0.1, 0.1, 0.8, 0.8], polar=True)
    N = 20
    theta = np.arange(0.0, 2*np.pi, 2*np.pi/N)
    radii = [10,10,11,9,10,10,10,11,9,10,5,3,4,3,4,5,10,10,10,10]#10*np.random.rand(N)
    width = np.pi/4*np.random.rand(N)
    bars = ax.bar(theta, radii, width=width, bottom=0.0)
    for r,bar in zip(radii, bars):
        bar.set_facecolor( plt.cm.jet(r/10.0))
        bar.set_alpha(0.5)
    
    plt.show()


#PARTIE GRAPHIQ   
def create_grid(event=None):
    w = c.winfo_width() # Get current width of canvas
    h = c.winfo_height() # Get current height of canvas
    c.delete('grid_line') # Will only remove the grid_line

    # Creates all vertical lines at intevals of 100
     
     #Creation contour cible
    c.create_polygon(zip(x_cible,y_cible), outline="black", fill='yellow')
    
    #Creation terrain
    c.create_polygon(zip(x,y), outline="black", fill='white')
    #Obstacle circulaire  
    indice =0
    m = 0
    for i in range(len( terrain['obstacles'] )-1 ):
        
        if(terrain['obstacles'][i]['type'] == 'cercle'):
            c.create_oval(obstaclex[i]-rayon[i],obstacley[i]-rayon[i],obstaclex[i]+rayon[i],obstacley[i]+rayon[i], outline="black", fill='black')
     
        if(terrain['obstacles'][i]['type'] == 'polygone'):
            c.create_polygon(zip(obstacle_polX[m:m+ taille_Pol[indice]],obstacle_polY[m:m+ taille_Pol[indice]]), outline="black", fill='black')
            m = m + taille_Pol[indice]
            indice = indice + 1
           #Obstable polyg
      
        
        
    #cible
    c.create_rectangle(xc-10,yc-10,xc+10,yc+10, outline="black", fill='pink')
    #étape
    for i in range(len( etapeX )):
       
        #debut
        if i == 0:
            c.create_rectangle(etapeX[i]-10,etapeY[i]-10,etapeX[i]+10,etapeY[i]+10, outline="black", fill='red')
        #fin
        elif i == len( etapeX )-1:
            c.create_rectangle(etapeX[i]-10,etapeY[i]-10,etapeX[i]+10,etapeY[i]+10, outline="black", fill='green')
        else:
            c.create_rectangle(etapeX[i]-10,etapeY[i]-10,etapeX[i]+10,etapeY[i]+10, outline="black", fill='cyan')



Frame1 = tk.Frame(root, borderwidth=2, relief=tk.GROOVE)
Frame1.pack(side=tk.LEFT, padx=30, pady=30)


Frame3 = tk.Frame(Frame1, borderwidth=2, relief=tk.GROOVE)
Frame3.pack(side=tk.LEFT, padx=30, pady=30)

Frame2 = tk.Frame(Frame1, borderwidth=2, relief=tk.GROOVE)
Frame2.pack(side=tk.BOTTOM, padx=30, pady=30)

bouton= tk.Button(Frame3, text="CHARGER CHEMIN (JSON)", command=chemin)
bouton.pack(side=tk.TOP)
boutonFP= tk.Button(Frame3, text="FAIRE PARCOURS", command=send_serie)
boutonFP.pack(side=tk.TOP)

boutonLumiere= tk.Button(Frame2, text="Tirer sur cible", command=calcul_angles_pointeur)
boutonLumiere.pack(side=tk.LEFT)

boutonLumiere= tk.Button(Frame3, text="DISTANCE PAR BALYAGE", command=graphdist)
boutonLumiere.pack(side=tk.BOTTOM)



labelx = tk.Label(Frame2, text="X")
labelx.pack(side=tk.LEFT)
entrex = tk.Entry(Frame2)
entrex.pack(side=tk.LEFT)
labely = tk.Label(Frame2, text="Y")
labely.pack(side=tk.LEFT)
entrey = tk.Entry(Frame2)
entrey.pack(side=tk.LEFT)
labela = tk.Label(Frame2, text="angle")
labela.pack(side=tk.LEFT)
entrea = tk.Entry(Frame2)
entrea.pack(side=tk.LEFT)

Lenergie = tk.Label(Frame3, textvariable=varenergie)
Lenergie.pack(side=tk.BOTTOM)

LposRobot = tk.Label(Frame3, textvariable=posRobot)
LposRobot.pack(side=tk.BOTTOM)

Laction_encours = tk.Label(Frame3, textvariable=action_encours)
Laction_encours.pack(side=tk.BOTTOM)


info = tk.Label(Frame3, text="INFORMATION")
info.pack(side=tk.BOTTOM)
boutonE= tk.Button(Frame2, text="Ajouter coord", command=getorder)
boutonE.pack(side=tk.LEFT)



c = tk.Canvas(Frame1, height=1000, width=1000, bg='grey')
c.pack(fill=tk.BOTH, expand= True)

c.bind('<Configure>', create_grid)


##########################################################
######### A STAR ALGORTITHM ##############################
def heuristic(a, b):
    return (b[0] - a[0]) ** 2 + (b[1] - a[1]) ** 2

def astar(array, start, goal):

    neighbors = [(0,1),(0,-1),(1,0),(-1,0),(1,1),(1,-1),(-1,1),(-1,-1)]

    close_set = set()
    came_from = {}
    gscore = {start:0}
    fscore = {start:heuristic(start, goal)}
    oheap = []

    heappush(oheap, (fscore[start], start))
    
    while oheap:

        current = heappop(oheap)[1]

        if current == goal:
            data = []
            while current in came_from:
                data.append(current)
                current = came_from[current]
            return data

        close_set.add(current)
        for i, j in neighbors:
            neighbor = current[0] + i, current[1] + j            
            tentative_g_score = gscore[current] + heuristic(current, neighbor)
            if 0 <= neighbor[0] < array.shape[0]:
                if 0 <= neighbor[1] < array.shape[1]:                
                    if array[neighbor[0]][neighbor[1]] == 1:
                        continue
                else:
                    # array bound y walls
                    continue
            else:
                # array bound x walls
                continue
                
            if neighbor in close_set and tentative_g_score >= gscore.get(neighbor, 0):
                continue
                
            if  tentative_g_score < gscore.get(neighbor, 0) or neighbor not in [i[1]for i in oheap]:
                came_from[neighbor] = current
                gscore[neighbor] = tentative_g_score
                fscore[neighbor] = tentative_g_score + heuristic(neighbor, goal)
                heappush(oheap, (fscore[neighbor], neighbor))
                
    return False



root.mainloop()

