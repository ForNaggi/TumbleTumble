:: RunServer.bat
@echo off
start "" "C:\Unreal\TumbleTumble\Binaries\Win64\TumbleTumbleServer.exe" MainMap -log -nosteam
timeout /T 3
start "" "C:\Unreal\TumbleTumble\Binaries\Win64\TumbleTumble.exe" 127.0.0.1 -windowed -ResX=800 -ResY=600 -log 
start "" "C:\Unreal\TumbleTumble\Binaries\Win64\TumbleTumble.exe" 127.0.0.1 -windowed -ResX=800 -ResY=600 -log 
