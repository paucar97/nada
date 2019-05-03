--Arian Gabriel Gallardo Callalli
--20153227
--Sistemas operativos 2018-1 | Laboratorio 2
--Pregunta 1: watermol.adb
--Solucionado usando protected objects

with Ada.Text_IO, Ada.Integer_Text_IO;
use Ada.Text_IO, Ada.Integer_Text_IO;

procedure watermol is
	protected type water is
		entry makeH;
		entry makeO;
		entry makeWater;
	private
		cantH: natural := 0;
		cantO: natural := 0;
	end water;

	protected body water is
		entry makeH when cantH < 2 is
		begin
			Put("Incremented H to: ");
			cantH:= cantH + 1;
			Put(cantH);
			Put_Line(" ");
		end;

		entry makeO when cantO < 1 is
		begin
			Put("Incremented O to: ");
			cantO:= cantO + 1;
			Put(CantO);
			Put_Line(" ");
		end;

		entry makeWater when (cantH = 2) and (cantO = 1) is
		begin
			Put_Line("One mol of water was made");
			cantH := 0;
			cantO := 0;
		end;
	end water;

	w: water;
	task hReady;
	task oReady;
	task makeWater;

	task body hReady is
	begin
		loop
			w.makeH;
		end loop;
	end;

	task body oReady is
	begin
		loop
			w.makeO;
		end loop;
	end;

	task body makeWater is
	begin
		loop
			w.makeWater;
		end loop;
	end;

begin
	null;
end watermol;
