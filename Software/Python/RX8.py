#!/usr/bin/python
# -*- coding: utf-8 -*-
########################################
# Programme pour enregistrer les données d'un capteur de pression pour faire le diagnostique
# d'un moteur rotatif
########################################

from Tkinter import *
import serial
import tkFileDialog
import sys
import glob
import time


########################################
#Efface le graphique et crée les lignes avec les unités
########################################
def clear_graph():
	canvas.delete(ALL)
	canvas.create_text(25,100,text= "100PSI")
	canvas.create_line(50,100,900,100)
	canvas.create_text(25,200-145,text= "145PSI")
	canvas.create_line(50,200-145,900,200-145)
	canvas.create_text(25,200-60,text= " 60PSI")
	canvas.create_line(50,200-60,900,200-60)

########################################
# Ouvre la fenetre pour demander ou sauvegarde l'aquisition
######################################## 
def Record_as():
	global filesave
	filesave=tkFileDialog.asksaveasfilename(title="Enregistrer l'aquisition sous",filetypes=[('txt files','.txt'),('all files','.*')])

def Load_file():
	return 0


########################################
# Renvoie la liste des port com disponible
######################################## 
def List_Port():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result

########################################
# Ouvre le port Com et enregistre les valeurs dans un fichier
########################################  
def Aquisitionportcom():
	global PortSerie
	global filesave
	global EtatCom

	clear_graph()
	old=200

	fin = str(value.get()) #recupére la durée en seconde demander

	#Si aucun fichier n'a été défini pour la sauvegarde ouvre la fenetre de demande
	if filesave=='' or filesave==None:
		Record_as()

	print filesave
	fichier=open(filesave,"a") # Ouverture du fichier en mode Append

	# Si le port Com n'a pas été initialisé, on lance la procédure d'ouverture du port Com
	if EtatCom == "Close":
		OpenCom()

	FinAquisition=time.time()+int(fin)

	while time.time() < FinAquisition:
		clear_graph()
		for i in range(0,850):
			new=PortSerie.readline()
			fichier.write(new)
			Tms=new[new.find(';')+1:-1]
			Val= new[0:new.find(';')]
			print Tms
			print Val
			print new
			canvas.create_line(50+i,int(old),i+51,int(200-float(Val)))
			old=int(200-float(Val))
			canvas.update()
		
			
	fichier.close()




########################################
# Ouvre le port Com choisi
########################################
def OpenCom():
        global PortSerie
	global ComPortNumber

	PortSerie = serial.Serial(ComPortNumber.get())
        PortSerie.close()
        PortSerie.baudrate = 115200
        PortSerie.databits = 8
        PortSerie.open()
	

########################################
# Ferme le port com
########################################
def CloseCom():
        global PortSerie

        PortSerie.close()           

########################################
# Change d'état le port Com lorsqu'on appuis sur le bouton
########################################
def ChangeCom():
	global EtatCom
	global ComPortNumber

	if EtatCom == "Close":
		OpenCom()
        	EtatCom="Open"
		print "Port com " + ComPortNumber.get() +" Ouvert"
	else:
		CloseCom()
		EtatCom="Close"
		print "Port com " + ComPortNumber.get() +" fermer"

########################################
#Boucle principale, crée la fenetre
########################################
def main():
        global canvas
        global filesave
	global ComPortNumber
	global EtatCom
	global PortSerie
	global value
	

	EtatCom = "Close"
        fenetre = Tk()
        filesave=""
        Frame_Graphique = Frame(fenetre)
        
        canvas = Canvas(Frame_Graphique, width=900, height=200, background= 'white')
        Frame_Option = Frame(fenetre)
        
        clear_graph()
        canvas.pack()

        bouton_A= Button(Frame_Option, text= "Aquisition", command=Aquisitionportcom)
	bouton_Com = Button(Frame_Option, text= "Ouvrir", command = ChangeCom)

        menubar= Menu(fenetre)
        menufichier=Menu(menubar,tearoff=0)
        #menufichier.add_command(label="Charger",command=Load_file)
        menufichier.add_command(label="Choix fichier de sauvegarde",command=Record_as)
        menufichier.add_separator()
        menufichier.add_command(label="Effacer",command=clear_graph)
        menufichier.add_command(label="Quitter",command=fenetre.quit)

        menubar.add_cascade(label="Fichier", menu=menufichier)
        

        label1=Label(Frame_Option,text="Duree en seconde")
    
        value = StringVar() 
        value.set("10")
        entree = Entry(Frame_Option, textvariable=value, width=30)

	portdeCom=List_Port()
	ComPortNumber = StringVar(fenetre)
        ComPortNumber.set(portdeCom[0]) # default value

        listePortCom = apply(OptionMenu, (Frame_Option, ComPortNumber)+ tuple(portdeCom))
        listePortCom.pack(side= RIGHT, padx=0, pady=0)
       
        fenetre.config(menu=menubar)

        #Positionnement des widget de la frame Option
	#bouton_Com.pack(side= RIGHT, padx=20, pady=5)
        bouton_A.pack(side= LEFT, padx=5, pady=5)
        label1.pack(side= LEFT, padx=15, pady=5)
        entree.pack(side= LEFT, padx=55, pady=5)

        Frame_Graphique.pack()
        Frame_Option.pack()
        fenetre.title("Mesure de Compression Rotatif")
        fenetre.mainloop()

########################################
#Lancement du script
########################################
if __name__ == '__main__':
    main()
t