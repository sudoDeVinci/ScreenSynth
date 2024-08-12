from vga import draw_fastHline, fill_rect, draw_circle, fill_disk, draw_rect, draw_pix, draw_checker
from vga import a0, a1
from gc import mem_free, collect
from micropython import const
from struct import unpack, pack, calcsize
from array import array
from os import listdir, ilistdir

# 3 bit color names
BLACK   = const(0b000)
RED     = const(0b001)
GREEN   = const(0b010)
BLUE    = const(0b100)
YELLOW  = const(0b011)
WHITE   = const(0b111)
CYAN    = const(0b110)
MAGENTA = const(0b101)
format_specifier = const("IBBB")
struct_size = calcsize(format_specifier)

# Drawing Various figures
draw_checker()
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


def read_frames(folder_path: str):
    """
    Stream frames out from files.
    """
    data_array = array('B')
    files = tuple("/".join([folder_path, filename]) for filename in listdir(folder_path) if filename.endswith('.csv'))
    for filename in files:
        
        with open(filename, 'r', encoding = "utf-8") as csvfile:
            next(csvfile)
            for line in csvfile:
                i, a, b ,c = line.rstrip('\n').rstrip('\r').split(",")
                packed_data = pack(format_specifier,
                                   int(i), 
                                   int(a), 
                                   int(b),
                                   int(c))
                data_array.extend(packed_data)
    
    return data_array


def render(data_array) -> None:
    # Iterate over the array in chunks of the struct size
    for i in range(0, len(data_array), struct_size):
        unpacked_data = unpack(format_specifier, data_array[i:i + struct_size])
        pass
    #TODO
    return 0


before = mem_free()
frame_data = read_frames("/")
render(frame_data)
collect()


print(f"mem used by visual buffer array (kB):\t{(a0 - a1)/8024:.3f}")
print(f"mem used by frame array: (kB):\t{(before-mem_free())/8024:.3f}")
print(f"mem free (kB):\t{mem_free()/8024:.3f}")