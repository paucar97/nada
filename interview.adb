--Arian Gabriel Gallardo Callalli
--20153227
--Sistemas operativos 2018-1 | Laboratorio 2
--Pregunta 3: interview.adb
--Solucionado usando rendezvous

with Ada.Text_IO, Ada.Integer_Text_IO;
use Ada.Text_IO, Ada.Integer_Text_IO;

procedure interview is

	task AnswerStart is
		entry qS;
	end;

	task AnswerDone is
		entry aS;
	end;

	task QuestionStart is
		entry qD;
	end;

	task QuestionDone is
		entry aD;
	end;

	task body QuestionStart is
	begin
		loop
			Put_Line("Student starts asking a question, and to pay attention");
			AnswerStart.qS;
			accept qD do
				null;
			end;
		end loop;
	end;

	task body AnswerStart is
	begin
		loop
			accept qS do
				null;
			end;
			Put_Line("Professor starts answering the question");
			AnswerDone.aS;
		end loop;
	end;

	task body AnswerDone is
	begin
		loop
			accept aS do
				null;
			end;
			Put_Line("Professor ended answering the question");
			QuestionDone.aD;
		end loop;
	end;

	task body QuestionDone is
	begin
		loop
			accept aD do
				null;
			end;
			Put_Line("Student received the answer and ends paying attention");
			QuestionStart.qD;
		end loop;
	end;

begin
	null;
end;
