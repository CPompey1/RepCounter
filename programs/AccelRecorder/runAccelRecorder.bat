@echo off
setlocal enabledelayedexpansion

REM Find the highest trial number in the directory
set max=0
for %%f in (trial*.txt) do (
    set fname=%%~nf
    set num=!fname:trial=!
    REM Check if num is a number
    for /f "delims=" %%n in ("!num!") do (
        if %%n gtr !max! set max=%%n
    )
)

set /a next=max+1
set outfile=trial%next%.txt

REM Run PlatformIO, show output in console AND save to the next trial file
powershell -NoProfile -ExecutionPolicy Bypass -Command "& 'C:\Users\cristian\.platformio\penv\Scripts\platformio.exe' run --target upload --target monitor --environment nanorp2040connect 2>&1 | Tee-Object -FilePath '%outfile%'"

echo Output saved to %outfile%
endlocal