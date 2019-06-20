import tkinter as tk
import tkinter.filedialog as fd
import tkinter.messagebox as mb

def message(titulo,mensaje):
    mb.showinfo(titulo,mensaje)
"""
def scale():
    slider_1 = Scale(self,).pack()
"""
class App(tk.Tk):
     def __init__(self):
         super().__init__()
         self.fname = None
         self.vfat= 0
         self.title("Visor File System FAT16")         
         self.canvas = tk.Canvas(self, width=400, height=280, bg="white")         
         btn_file = tk.Button(self, text="Choose file system image",command=self.choose_file)         
         btn_showBoot = tk.Button(self, text="Boot Sector",command=self.show_bootsector)
         btn_showFat = tk.Button(self, text="File Allocation Table",command=self.show_fat)
         btn_showRootDir = tk.Button(self, text="Root directory",command=self.show_rootdir)                  
         btn_file.grid(row=0, column=0, columnspan=2, padx=60,pady=20 )
                 
         btn_showBoot.grid(row=2, column=0, padx=60,pady=20)
         btn_showFat.grid(row=2, column=1, padx=60,pady=20)
         btn_showRootDir.grid(row=3, column=1, padx=60,pady=20)
         self.canvas.grid(row=4, column=0,rowspan=3, columnspan=2)
         
           
     def choose_file(self):
         filetypes = (("File sistem images", "*.img"),("All files", "*"))
         filename = fd.askopenfilename(title="Open file", initialdir="/home/alejandro/SourceCode/Python", filetypes=filetypes)
         self.fname = filename
         f = open(filename,'rb')
         #l = int.from_bytes(f.read(4),byteorder='little')
         f.seek(54)
         l = str(f.read(8))
         print(l)
         f.seek(0)
         f.seek(19)
         print(int.from_bytes(f.read(2),byteorder='little'))
         
         
         if 'FAT' not in l:
            message('titulo','error no es fat')
            self.fname = None
         else:
             if 'FAT12' in l:
                 self.vfat= 12
             elif 'FAT16' in l:
                 self.vfat= 16
             else:
                 self.vfat= 32
         f.seek(0)
               
               
     def show_bootsector(self):
         if self.fname == None:
             self.show_error1()
         else:    
             with open(self.fname,mode='rb') as file:
                  self.canvas.delete("all")
                  file.seek(0)
                  inodes=file.read(4)
                  i=int.from_bytes(inodes,byteorder='little')
                  self.canvas.create_text(20,20, text="Numero de inodos en el sistema de archivos: "+str(i),anchor=tk.W)
                  blocks=file.read(4)
                  i=int.from_bytes(blocks,byteorder='little')
                  self.canvas.create_text(20,40, text='Numero de bloques en el sistema de archivos: '+str(i),anchor=tk.W)
                  reserved_blocks=file.read(4)
                  i=int.from_bytes(reserved_blocks,byteorder='little')
                  self.canvas.create_text(20,60, text='Numero de bloques reservados en el sistema de archivos: '+str(i),anchor=tk.W)
                  free_blocks=file.read(4)
                  i=int.from_bytes(free_blocks,byteorder='little')
                  self.canvas.create_text(20,80, text='Numero de bloques libres: '+str(i),anchor=tk.W)
                  free_inodes=file.read(4)
                  i=int.from_bytes(free_inodes,byteorder='little')
                  self.canvas.create_text(20,100, text='Numero de inodos libres: '+str(i),anchor=tk.W)
                  first_data_block = file.read(4)
                  i=int.from_bytes(first_data_block,byteorder='little')
                  self.canvas.create_text(20,120, text='Bloque donde inicia grupo 0: '+str(i),anchor=tk.W)
                  log_block_size=file.read(4)
                  i=int.from_bytes(log_block_size,byteorder='little')                  
                  self.canvas.create_text(20,140, text='Tamano de bloque: '+str(i),anchor=tk.W)
                  file.seek(4,1)
                  blocks_per_group=file.read(4)
                  i=int.from_bytes(blocks_per_group,byteorder='little')
                  self.canvas.create_text(20,160, text='Numero de bloques en cada grupo: '+str(i),anchor=tk.W)
                  file.seek(4,1)
                  inodes_per_group=file.read(4)
                  i=int.from_bytes(inodes_per_group,byteorder='little')
                  self.canvas.create_text(20,180, text='Numero de inodos en cada grupo: '+str(i),anchor=tk.W)
                  file.seek(12,1)
                  magic=file.read(2)
                  i=int.from_bytes(magic,byteorder='little')
                  self.canvas.create_text(20,200, text='Signatura: '+str(hex(i)),anchor=tk.W)
                  file.seek(26,1)
                  first_ino = file.read(4)
                  i=int.from_bytes(first_ino,byteorder='little')
                  self.canvas.create_text(20,220, text='Primer inodo no reservado en el sistema de archivo: '+str(i),anchor=tk.W)
                  ino_size=file.read(2)
                  i=int.from_bytes(ino_size,byteorder='little')
                  self.canvas.create_text(20,240, text='Tamano del inodo: '+str(i),anchor=tk.W)
                  file.seek(30,1)
                  name = file.read(16)
                  n=name.decode()
                  self.canvas.create_text(20,260, text='Nombre del volúmen: '+n,anchor=tk.W)
         
     def show_fat(self):
         if self.fname == None:
             self.show_error1()
         else:
             self.canvas.delete("all")
             w = self.canvas.winfo_width() 
             h = self.canvas.winfo_height()
             self.canvas.delete(tk.ALL)
             with open(self.fname,mode='rb') as file:              
                  file.seek(self.vfat * 128)
                  ## Lee solo las primeras 16 entradas de la FAT
                  fat = []
                  for i in range(512):
                      byte=file.read(2)
                      i=int.from_bytes(byte,byteorder='little')
                      fat.append(i)            
                  cont = 0
                  for j in range(0,h,20):
                      for i in range(0,w,20):
                          if fat[cont] != 0:
                              self.canvas.create_rectangle(i,j,i+20,j+20,fill="yellow", outline = 'black')
                          else: 
                              self.canvas.create_rectangle(i,j,i+20,j+20,fill="white", outline = 'black') 
                          cont += 1
                          if cont == 512:
                              return
                        
     def show_rootdir(self):
            pass
                       
     def show_error1(self):
            msg = "No se ha elegido la imagen del sistema de archivos"
            mb.showerror("Error", msg)
            
     def show_error2(self):
            msg = "El archivo no contiene imagen de sistema de archivos Ext2"
            mb.showerror("Error", msg)
            
if __name__ == "__main__":
       app = App()
       app.mainloop()