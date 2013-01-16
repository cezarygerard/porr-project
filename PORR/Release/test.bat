@echo off

ECHO  XXTE MPI TEST > wyniki.txt
ECHO  XXTE MPI TEST >> wyniki.txt

set textSize=64;
set maxTextSize=1000000000;
set numberOfThreads=1;
set maxNumberOfThreads=16;

ECHO  %textSize% >> wyniki.txt
ECHO  %maxTextSize% >> wyniki.txt
ECHO   %numberOfThreads% >> wyniki.txt
ECHO %maxNumberOfThreads% >> wyniki.txt

:loop_outer
IF %textSize%>%maxTextSize% GOTO end_outer
	ECHO  %textSize% >> wyniki.txt
	:loop_inner
	IF %numberOfThreads%>%maxNumberOfThreads% GOTO end_inner
		ECHO  %textSize% >> wyniki.txt
		
		ECHO textSize: %textSize% maxNumberOfThreads: %maxNumberOfThreads% >> wyniki.txt
		mpiexec -n %numberOfThreads% MPI.exe %textSize% >> wyniki.txt
		
		set numberOfThreads=%numberOfThreads%+1;
		GOTO LOOP_INNER
	:end_inner
	set numberOfThreads=1;
	set textSize=%textSize%*2;
GOTO LOOP_OUTER
:end_outer

