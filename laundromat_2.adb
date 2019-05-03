with Ada.text_IO; use Ada.Text_IO;
with Ada.Integer_text_IO; use Ada.Integer_text_IO;

procedure Laundromat is 
	
	arregloDisponibe :array(1..8) of Boolean :=(True,True,True,True,True,True,True,True);
	procedure ciclo(id: in Integer) is
	begin 
	for i in Integer range 1..500 loop
		null; 
	end loop;
	    
	end ciclo;

	protected type controlador is 
		entry clienteCoin(id: in out Integer) ;
		procedure liberar(id : in Integer);
	private
		
		nfree : Integer := 8;
	end controlador; 


	protected body controlador is

		entry clienteCoin(id : in out Integer)  when nfree > 0    is
		begin
			
			for i in Integer range 1..8 loop
				if arregloDisponibe(i)=True then
					put_line("Assigment ..  machine " & Integer'Image(i));
					id := i;
					nfree := nfree -1;
					arregloDisponibe(i):=False;
					exit;
				end if;
			end loop;
		end;

		procedure liberar (id: in Integer)is
		begin
			arregloDisponibe(id):=True;
			put_line("Release ..  machine " & Integer'Image(id));
			nfree := nfree +1;
		end; 
	end controlador;



	c: controlador;
	task  type cliente1;
	--task cliente2;

	task body cliente1 is
	id:Integer:=0; 
	begin
			c.clienteCoin(id);
			
			c.liberar(id);
	end;

	--task body cliente2 is
	--id:Integer:=0; 
	--begin
	--		c.clienteCoin(id);
	--		c.liberar(id);		
	--end;
	cholas : array (1 .. 30) of cliente1;
	
	begin 

	null;
	end Laundromat;