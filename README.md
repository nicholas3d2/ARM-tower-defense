# ARM-tower-defense
Open Software Project for DE1-SoC board, developed for/using CPUlator's ARMv7 DE1-SoC. 

Copyright (C) 2021 Nicholas Ishankov, Jintao Huang, all rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

# 1.0 Description
Tower Defense is a type of strategy game where the goal is to stop the enemies from reaching the end of the path. The user places towers that shoot at enemies once they are in range to get points. The user spends points to place more towers. The game has three waves, each getting harder. 

<img src="https://user-images.githubusercontent.com/61950668/116820516-9e2f7180-ab43-11eb-8271-b8b4067b3aa6.PNG" width="90%"></img> 

# 2.0 Instructions

2.1 How to run

Load file (main.c) into [CPULATOR](https://cpulator.01xz.net/?sys=arm-de1soc&d_audio=48000), change language to C, compile and run. Click KEY3 to start the game

2.2 Controls

LEDR: Health bar. Users start off with 10 health. Losses one health for each enemy that reaches the end 

<img src="https://user-images.githubusercontent.com/61950668/116820188-a4bce980-ab41-11eb-98a2-0d85f5d1f9eb.PNG" width="90%"></img>

KEYS: User Controls.

<img src="https://user-images.githubusercontent.com/61950668/116820222-dd5cc300-ab41-11eb-8108-0b2eaf44391a.PNG" width="90%"></img> 

KEY0= Place Light Tower ( cost 25 points )

KEY1= Place Medium Tower ( cost 50 points )

KEY2= Place Heavy Tower ( cost 100 points )

KEY3= Start / Pause

7 Seg-Display: Displays the number of points for the user, they start with 75.

<img src="https://user-images.githubusercontent.com/61950668/116820389-d2566280-ab42-11eb-8d95-695ba42d951e.PNG" width="90%"></img> 

JTAG UART: Moves the cursor (user grid square) around on the display screen. Hovering over a tower shows its range. 

<img src="https://user-images.githubusercontent.com/61950668/116820296-46443b00-ab42-11eb-9df8-8c30b59bac46.PNG" width="90%"></img> 

w - up

s - down

a - left

d - right

VGA: Displays the game

<img src="https://user-images.githubusercontent.com/61950668/116820415-f619a880-ab42-11eb-96dc-0867cc13f0f5.PNG" width="90%"></img> 

Interval Timer: Controls the tower fire rates and enemy spawn rates. Each wave contains 15 enemies. 

<img src="https://user-images.githubusercontent.com/61950668/116820344-83a8c880-ab42-11eb-90a3-3ce1686da1e1.PNG" width="90%"></img> 

2.3 Game

Towers:

<img src="https://user-images.githubusercontent.com/61950668/116820608-f797a080-ab43-11eb-8b9d-78b393e60f30.PNG" width="10%"></img> 

Light Tower: costs 25, light damage, low range, high fire rate

<img src="https://user-images.githubusercontent.com/61950668/116820690-3decff80-ab44-11eb-8542-0a290e140c1c.PNG" width="10%"></img> 

Medium Tower: costs 50, medium damage, medium range, medium fire rate

<img src="https://user-images.githubusercontent.com/61950668/116820784-b18f0c80-ab44-11eb-8932-a4981a617f86.PNG" width="10%"></img> 

Heavy Tower: costs 100, high damage, high range, low fire rate

Enemies:

<img src="https://user-images.githubusercontent.com/61950668/116820912-6b867880-ab45-11eb-9e3a-9478641a336d.PNG" width="10%"></img> 

Light Enemies: 5 points,  light health, fast speed

<img src="https://user-images.githubusercontent.com/61950668/116820956-9b358080-ab45-11eb-916a-7f9e9d05459c.PNG" width="10%"></img> 

Medium Enemies: 15 points, medium health, fast speed

<img src="https://user-images.githubusercontent.com/61950668/116820859-34b06280-ab45-11eb-8bfc-8aed642bfbe7.PNG" width="10%"></img> 

Heavy Enemies: 25 points, high health, slow speed

Waves:

Wave 1: Random enemies spawn, 15 in total

Wave 2: Heavy enemies only, 15 in total

Wave 3: Heavy enemies only with double health, 15 in total. Game ends after this wave
