@echo off
@title extra .7z

cd  /d  %~dp0

if exist .\wwise (
   echo "wwise ... found"
) else (
	echo "wwise ... not found.. extra wwise.7z"
	.\7z.exe x .\wwise.7z -aoa
) 
