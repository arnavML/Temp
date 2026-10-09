/*
██████████████████████████████████████████████████████████████████████████████████████████████████████████
██                                                                                                      ██
██  ██████╗  █████╗ ███████╗ █████╗ ███████╗██╗         ██╗    ██╗██████╗  ██████╗ ████████╗ ███████╗   ██
██  ██╔══██╗██╔══██╗██╔════╝██╔══██╗██╔════╝██║         ██║    ██║██╔══██╗██╔═══██╗╚══██╔══╝ ██╔════╝   ██
██  ██████╔╝███████║█████╗  ███████║█████╗  ██║         ██║ █╗ ██║██████╔╝██║   ██║   ██║    █████╗     ██
██  ██╔══██╗██╔══██║██╔══╝  ██╔══██║██╔══╝  ██║         ██║███╗██║██╔══██╗██║   ██║   ██║    ██╔══╝     ██
██  ██║  ██║██║  ██║██║     ██║  ██║███████╗███████╗    ╚███╔███╔╝██║  ██║╚██████╔╝   ██║    ███████╗   ██
██                                                                                                      ██
██                         RAFAEL WROTE THIS FILE                                                       ██
██████████████████████████████████████████████████████████████████████████████████████████████████████████
*/

extern volatile char *VGA;

void draw_point(char color, int x, int y, int t){
    if(x >= 0 && x < (321-t) && y >= 0 && y < (241-t)){ //draws a point at x,y with thickenss t by for looping through the thickness(kinda like a square)
        for(int i = 0; i < t; i++){
            for(int j = 0; j < t; j++){
                VGA[(y+i)*320 + (x+j)] = color;
            }
        }
    }
    else if((0 <= x+t &&  x < 320) || (0 <= y+t &&  y < 240)){ //this is a check if the point is only partially out of bounds it prints the parts out of bounds
        int x0 = x < 0 ? 0 : x;
        int y0 = y < 0 ? 0 : y;
        int x1 = (x + t > 320) ? 320 : x + t;
        int y1 = (y + t > 240) ? 240 : y + t;
        for(int i = y0; i < y1; i++){
            for(int j = x0; j < x1; j++){
                VGA[i*320 + j] = color;
            }
        }
    }
}

void draw_rect(char color, int x1, int y1, int x2, int y2, int t, int fill){ // t = thickness 
    for(int i = x1; i < x2; i += t){ //draws the x boundaries
        draw_point(color, i, y1, t); 
        draw_point(color, i, y2-t, t); 
    } 

    for(int i = y1; i < y2; i += t){ //draws the y boundaries
        draw_point(color, x1, i, t); 
        draw_point(color, x2-t, i, t); 
    } 

    // fill
    if(fill){ //fill the rect
        for(int i = x1+t; i < x2-t; i += t){ 
            for(int j = y1+t; j < y2-t; j += t){ 
                draw_point(color, i, j, t); 
            } 
        } 
    } 
}

void draw_cursor(int x, int y) { //specific point placement for the cursor.
    char o = 0x00; // black
    char f = 0xFF; // white

    // outline
    draw_point(o,x-1,y+1,1);
    draw_point(o,x-1,y+2,1);
    draw_point(o,x-1,y+3,1);
    draw_point(o,x-1,y+4,1);
    draw_point(o,x-1,y+5,1);
    draw_point(o,x-2,y+5,1);
    draw_point(o,x-3,y+4,1);
    draw_point(o,x-4,y+4,1);
    draw_point(o,x-4,y+3,1);
    draw_point(o,x-3,y+2,1);
    draw_point(o,x-2,y+1,1);

    // fill
    draw_point(f,x-2,y+2,1);
    draw_point(f,x-2,y+3,1);
    draw_point(f,x-3,y+3,1);
    draw_point(f,x-2,y+4,1);
}