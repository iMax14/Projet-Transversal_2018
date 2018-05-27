from math import *

#Cette fonction permet de calculer la distance que doit parcourir le robot.
#On suppose dans cette fonction que l'angle initial du robot est nul.

#Pour se mettre dans le cas où cet angle est non nul, il suffirait d'envoyer
#l'angle actuel du robot, et avant de calculer le nouvel angle (cf. calculs ci-dessous), il serait nécessaire
#de mettre le robot à un angle nul.
    ###################
    # Exemple de code :
#       if (angle_actuel <> 0):
#           angle = 360 - angle_actuel # Angle actuel devra être initialisé à 0 pour le premier calcul
    ###################

def calcul_parcours(x_initial,y_initial,x_final,y_final):

    if (x_initial < x_final) && (y_initial < y_final) :
        angle = atan(fabs(y_final - y_initial)/fabs(x_final-x_initial))

    if (x_initial < x_final) && (y_initial > y_final) :
        angle = 270 + (90 - atan(fabs(y_final - y_initial)/fabs(x_final-x_initial)))

    if (x_initial < x_final) && (y_initial < y_final) :
        angle = 90 + (90 - atan(fabs(y_final - y_initial)/fabs(x_final-x_initial)))

    if (x_initial < x_final) && (y_initial < y_final) :
        angle = 180 + atan(fabs(y_final - y_initial)/fabs(x_final-x_initial))

    distance = sqrt(fabs(y_final - y_initial)**2 + fabs(x_final-x_initial)**2)

    return angle + distance
