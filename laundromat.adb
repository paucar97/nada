--Arian Gabriel Gallardo Callalli
--20153227
--Sistemas operativos 2018-1 | Laboratorio 2
--Pregunta 2: Laundromat.adb
--Solucionado usando protected objects

with Ada.Text_IO, Ada.Integer_Text_IO;
use Ada.Text_IO, Ada.Integer_Text_IO;

procedure laundromat is	

	protected type laundry(N:natural) is
		entry allocate;
		entry release;
	private
		NMACHINES: Integer := N;
		nfree: Integer := N;
	end laundry;

	protected body laundry is
		entry allocate when nfree > 0 is
		begin
			Put_Line("Allocating machine!");
			nfree := nfree - 1;
		end;

		entry release when nfree < NMACHINES is
		begin
			Put_Line("Releasing machine!");
			nfree := nfree + 1;
		end;
	end laundry;

	L: laundry(5); --Probaremos con 5 maquinas

	task allocate;
	task release;
	PED: Integer := 3;

	task body allocate is
		x:Integer := 1;
	begin
		loop
			for i in Integer range 1 .. PED loop
				L.allocate;
			end loop;
			Put_Line("Finished allocating person: " & (Integer'Image(x)));
			x := x+1;
		end loop;
	end;

	task body release is
	begin
		loop
			L.release;
		end loop;
	end;

begin
	null;
end;
