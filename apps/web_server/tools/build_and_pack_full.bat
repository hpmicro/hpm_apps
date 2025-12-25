@echo off
@echo off
REM build_and_pack_full.bat - Robust build & pack script (ASCII-only)
REM Steps:
REM  1) Build frontend (npm run build)
REM  2) Pack web_app/dist into web_server/src/packed_fs.c using tools\make_packed_fs.py
REM  3) Optional: trigger firmware build (ninja)

setlocal enabledelayedexpansion

echo ========================================
echo Checking runtime dependencies: Node/npm, Python
echo ========================================

where node >nul 2>&1
if %errorlevel% NEQ 0 echo ERROR: node not found. Install from https://nodejs.org/ & pause & exit /b 1

where npm >nul 2>&1
if %errorlevel% NEQ 0 echo ERROR: npm not found. Ensure Node.js is installed and npm is in PATH & pause & exit /b 1

where python >nul 2>&1
if %errorlevel% NEQ 0 echo ERROR: python not found. Install Python 3 and ensure python is in PATH & pause & exit /b 1

echo.
echo ========================================
echo Step 1/2: Build frontend (web_app)
echo ========================================

rem Use script directory to locate web_app
set "SCRIPT_DIR=%~dp0"
set "WEB_APP_DIR=%SCRIPT_DIR%..\web_app"

if not exist "%WEB_APP_DIR%" echo ERROR: web_app directory not found at %WEB_APP_DIR% & pause & exit /b 1

pushd "%WEB_APP_DIR%"

if not exist node_modules goto :do_npm_install
goto :after_npm_check

:do_npm_install
echo node_modules not found, running npm install ...
call npm install
if errorlevel 1 echo ERROR: npm install failed & popd & pause & exit /b 1

:after_npm_check
echo Running: npm run build
call npm run build
if errorlevel 1 echo ERROR: frontend build failed & popd & pause & exit /b 1

popd

echo.
echo ========================================
echo Step 2/2: Pack into packed_fs.c
echo ========================================

rem Define paths relative to script location
set "SCRIPT_DIR=%~dp0"
set "INDIR=%SCRIPT_DIR%..\web_app\dist"
set "OUTC=%SCRIPT_DIR%..\web_server\src\packed_fs.c"
set "PYSCRIPT=%SCRIPT_DIR%make_packed_fs.py"
set "PREFIX=/web_root"

if exist "%INDIR%" (
  echo Input directory: %INDIR%
) else (
  echo ERROR: build output directory %INDIR% not found. Ensure frontend build succeeded
  pause
  exit /b 1
)

echo Running: python "%PYSCRIPT%" "%INDIR%" "%OUTC%" --prefix %PREFIX%
python "%PYSCRIPT%" "%INDIR%" "%OUTC%" --prefix %PREFIX%
if errorlevel 1 (
  echo ERROR: packing script failed
  pause
  exit /b 1
)

echo Pack succeeded, output: %OUTC%

echo.
echo Done (frontend built and packed).
pause
endlocal
exit /b 0
