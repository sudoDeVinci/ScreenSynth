from vga import *

# Drawing a simple 8 color checker
for h in range(8):
    for i in range(0,60):
        for k in range(8):
            col=(h+k)%8
            draw_fastHline(k*80,k*80+80,h*60+i,col)

# Drawing Various figures
fill_rect(20,20,150,150,BLACK)
fill_rect(20,200,150,400,BLUE)
fill_rect(200,205,205,150,WHITE)
fill_rect(300,415,350,300,YELLOW)
fill_rect(550,450,640,150,CYAN)
draw_circle(100,400,75,YELLOW)
draw_circle(150,150,98,CYAN)
fill_disk(320,240,150,BLACK)
fill_disk(320,240,120,RED)
fill_disk(320,240,80,GREEN)
fill_disk(320,240,50,WHITE)
draw_rect(500,50,620,70,BLACK)
draw_rect(100,390,600,480,RED)

draw_pix(320,239,WHITE)
draw_pix(320,240,WHITE)
draw_pix(320,241,WHITE)

mem_free()
collect()