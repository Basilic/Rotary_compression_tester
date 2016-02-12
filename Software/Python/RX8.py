#! python
#

from Tkinter import *
from serial import *
from serial.tools.list_ports import comports
import tkFileDialog

########################################
#
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
# 
######################################## 
def Record_as():
           file = tkFileDialog.asksaveasfilename(title="Ouvrir une image",filetypes=[('png files','.png'),('all files','.*')])
           print file
########################################
# 
########################################  
def Aquisitionportcom():
            clear_graph()
            old=200
            s = Serial('COM17')
            s.close()
            s.baudrate = 115200
            s.databits = 8
            s.open()
            fin = str(value.get())
            print file
            fichier=open(file,"a")
            #fichier=open("Text.txt","a")
            for j in range(0, int(fin)):
                clear_graph()
                for i in range(0,850):
                       new=s.readline()
                       fichier.write(new)
                       Tms=new[new.find(';')+1:-1]
                       Val= new[0:new.find(';')]
                       print Tms
                       print Val
                       print new
                       canvas.create_line(50+i,int(old),i+51,int(200-float(Val)*10))
                       old=int(200-float(Val)*10)
                       
            s.close()
            fichier.close()
            canvas.update()
            
########################################
#
########################################
def main():
        global fenetre
        global canvas
        global value
        global file
        fenetre = Tk()

        Frame_Graphique = Frame(fenetre)
        
        canvas = Canvas(Frame_Graphique, width=900, height=200, background= 'white')
        Frame_Option = Frame(fenetre)
        
        clear_graph()
        canvas.pack()

        bouton= Button(Frame_Option, text= "Aquisition", command=Aquisitionportcom)

        menubar= Menu(fenetre)
        menufichier=Menu(menubar,tearoff=0)
        menufichier.add_command(label="Charger",command=clear_graph)
        menufichier.add_command(label="Sauvegarder",command=Record_as)
        menufichier.add_separator()
        menufichier.add_command(label="Effacer",command=clear_graph)
        menufichier.add_command(label="Quitter",command=fenetre.quit)

        menubar.add_cascade(label="Fichier", menu=menufichier)
        fenetre.config(menu=menubar)

        label1=Label(Frame_Option,text="Duree en seconde")
    
        value = StringVar() 
        value.set("5")
        entree = Entry(Frame_Option, textvariable=value, width=30)

        #Positionnement des widget de la frame Option
        bouton.pack(side= LEFT, padx=5, pady=5)
        label1.pack(side= LEFT, padx=15, pady=5)
        entree.pack(side= LEFT, padx=55, pady=5)

        Frame_Graphique.pack()
        Frame_Option.pack()
        fenetre.mainloop()


########################################
#
########################################
if __name__ == '__main__':
    main()
