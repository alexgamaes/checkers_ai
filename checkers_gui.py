import tkinter as tk
import subprocess
import os
import signal

from tkinter import *
from tkinter import ttk, filedialog, messagebox, colorchooser
from copy import copy, deepcopy


global colour_selected, colour_possible_moves
colour_selected = "khaki"
colour_possible_moves = "orange"

LARGE_FONT = ("Verdana", 40)

                     
                     
#os.killpg(os.getpgid(p.pid), signal.SIGTERM) 

def start_engine():
    global p
    
    p = subprocess.Popen(['out/console_interface.exe'],
                     stdout=subprocess.PIPE,
                     stdin=subprocess.PIPE)
                     
    
start_engine()                 
def get_legal_movements_subprocess(board, x, y):
    global p

    data = 'get_legal_movements\n'
    data += "%d %d\n" % (x, y)
    
    for i in range(8):
        data += "%s\n" % board[i]
        
    try:
        outs, errs = p.communicate(bytes(data, 'ascii'), timeout=10)
        p.kill()
        
        lines = list(filter(None, outs.decode('ascii').split('|')))
        
        movements = []
        for m in lines[1:]:
            l = list(map(int, list(filter(None, m.split(' ')))))
            
            lis = []
            for i in range(0, len(l), 2):
                lis.append([l[i], l[i + 1]])
            
            movements.append(lis)
        
        
        start_engine()
        
        return movements
    except Exception as e:
        print(e)

    return []
    
"""                     
p.stdin.write(b'abc\n')
p.stdin.close()

print("Reading result 1:", p.stdout.readline().decode(encoding='ascii'))

exit(0)
"""

class symbols(object):
    b = ' '
    w = '#'
    bm = '+'
    bk = '*'
    wm = '-'
    wk = '%'  
    
def color_square(c, r):
    if (c + r) % 2 == 1:
        return symbols.w
    return symbols.b


class Board(object):
    # The chess board is represented as a 8x8 2D array
    def __init__(self):
        self._turn = "w"
        
        self.board = [
            symbols.b + symbols.bm + symbols.b + symbols.bm + symbols.b + symbols.bm + symbols.b + symbols.bm,
            symbols.bm + symbols.b + symbols.bm + symbols.b + symbols.bm + symbols.b + symbols.bm + symbols.b,
            symbols.b + symbols.bm + symbols.b + symbols.bm + symbols.b + symbols.bm + symbols.b + symbols.bm,
            symbols.w + symbols.b + symbols.w + symbols.b + symbols.w + symbols.b + symbols.w + symbols.b,
            symbols.b + symbols.w + symbols.b + symbols.w + symbols.b + symbols.w + symbols.b + symbols.w,
            symbols.wm + symbols.b + symbols.wm + symbols.b + symbols.wm + symbols.b + symbols.wm + symbols.b,
            symbols.b + symbols.wm + symbols.b + symbols.wm + symbols.b + symbols.wm + symbols.b + symbols.wm,
            symbols.wm + symbols.b + symbols.wm + symbols.b + symbols.wm + symbols.b + symbols.wm + symbols.b,
            ]
            
        self.legal_movements = None
 

    def legalmoves(self, from_coords, board):
        # print(from_coords)
        self.legal_movements = get_legal_movements_subprocess(self.board, from_coords[0], from_coords[1])
        
        return self.legal_movements
        
        
        
    def is_legal(self, from_coords):
        for i in range(0, len(self.legal_movements)):
            for j in range(0, len(self.legal_movements[i])):
                move = self.legal_movements[i][j]
                
                if move[0] == from_coords[0] and move[1] == from_coords[1]:
                    return True
                    
        return False
         
         
    def make_movement(self, row1, col1, row2, col2):
        top = min(row1, row2);
        bottom = max(row1, row2);
        left = min(col1, col2);
        right = max(col1, col2);
        
        ans = deepcopy(self.board)
        
        for i in range(8):
            ans[i] = list(ans[i])
        
        c = left + 1
        r = top + 1
        
        
        while c < right:
            if self.board[r][c] in [symbols.bm, symbols.bk, symbols.wm, symbols.wk]:
                ans[r][c] = color_square(r, c);
                break;
                
            r += 1
            c += 1
        
        ans[row2][col2] = ans[row1][col1]
        ans[row1][col1] = color_square(row1, col1)
        
        print(row1, col1, row2, col2)
        for i in range(8):
            ans[i] = "".join(ans[i])

        
        self.board = ans
        
        for i in range(8):
            #ans[i] = "".join(ans[i])
            print(self.board[i])
            
        
    def check_mate(self):
        whitePieces = False
        blackPieces = False
        
        
        for i in range(8):
            for j in range(8):
                if self.board[i][j] == symbols.wm or self.board[i][j] == symbols.wk:
                    whitePieces = True
                    
                if self.board[i][j] == symbols.bm or self.board[i][j] == symbols.bk:
                    blackPieces = True
                    
                    
        if whitePieces == False:
            return "w"
        elif blackPieces == False:
            return "b"
        return None

    def turn(self):
        return self._turn
        
    def next_turn(self):
        self._turn = 'w' if self._turn == 'b' else 'b'
    
board = Board()    
        
class CheckersApp(tk.Tk):

    def __init__(self, *args, **kwargs):
        tk.Tk.__init__(self, *args, **kwargs)

        #tk.Tk.iconbitmap(self, default = "chessgame.ico")
        tk.Tk.wm_title(self, "Checkers GAMA")

        container = tk.Frame(self)
        container.pack(side = "top", fill = "both", expand = True)
        container.grid_rowconfigure(0, weight = 1)
        container.grid_columnconfigure(0, weight = 1)

        global selected, piece_selected, square_selected
        selected = False
        square_selected = ""
        piece_selected = ""

        global game_played
        game_played = False


        self.frames = {}

        for F in [Game]:

            frame = F(container, self)

            self.frames[F] = frame

            frame.grid(row = 0, column = 0, sticky = "NSEW")

        self.show_frame(Game)


    def show_frame(self, cont):

        frame = self.frames[cont]
        frame.tkraise()

def Reset_Board(controller):
    global selected, square_selected, piece_selected, game_over
    game_over = False
    selected = False
    square_selected = ""
    piece_selected = ""

class Game(tk.Frame):

    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)

        loadimages()
        Reset_Board(controller)

        grid = Frame(self)
        grid.grid(sticky=N + S + E + W, column=0, row=7, columnspan=2)
        Grid.rowconfigure(self, 7, weight=1, minsize=70)
        Grid.columnconfigure(self, 0, weight=1, minsize=70)

        global board, stack, log
        global game_played
        game_played = True


        global Buttons, Colours
        Buttons = []
        Colours = []

        # Import images for pieces
        # where
        # colour + PIECE + .gif = file name

        for x in range(0, 8):
            Even = True
            if x % 2:
                Even = False
            Buttons.append([])
            Colours.append([])
            for y in range(0, 8):
                if Even:
                    btn = tk.Button(self, bg="white", image=Empty)
                    Colours[x].append("white")
                    Even = False

                else:
                    btn = tk.Button(self, bg="black", image=Empty)
                    Colours[x].append("black")
                    Even = True

                if board.board[y][x] != symbols.w and board.board[y][x] != symbols.b:
                    if board.board[y][x] == symbols.bm:
                        btn.configure(image=bM)
                    if board.board[y][x] == symbols.bk:
                        btn.configure(image=bK)

                    if board.board[y][x] == symbols.wm:
                        btn.configure(image=wM)
                    if board.board[y][x] == symbols.wk:
                        btn.configure(image=wK)

                btn.grid(column=x, row=y, sticky=N + S + E + W)
                Buttons[x].append(btn)

        for x in range(0, 8):
            Grid.columnconfigure(self, x, weight=1, minsize=70)

        for y in range(0, 8):
            Grid.rowconfigure(self, y, weight=1, minsize=70)

        global Turn_var
        
        Turn_var = StringVar()
        PlayerTurnLabel = tk.Label(self, textvariable=Turn_var ,font=30)
        PlayerTurnLabel.grid(column=10, row=0, sticky=(N,S))
        
        gap = ttk.Label(self, text=" "*25)
        gap.grid(column=9, row=0,rowspan=8, sticky=(N,S))

        gap2 = ttk.Label(self, text=" "*20)
        gap2.grid(column=16, row=1, rowspan=8, sticky=(N,S))

        log = Text(self, state='disabled',height=25, width=60, wrap="none",font=26)
        log.grid(column=10, row=1, rowspan=6,sticky=(N,S,E,W))



        button = ttk.Button(self, text = "Print Board", command = lambda: board.boardshow())
        button.grid(column=9, row=7,ipady=10,ipadx=10)

def PiecesImagesUpdate():
    for i in range(0, 8):
        for j in range(0, 8):
            btn = Buttons[i][j]
            if board.board[j][i] != symbols.w and board.board[j][i] != symbols.b:
                if board.board[j][i] == symbols.bm:
                    btn.configure(image=bM)
                if board.board[j][i] == symbols.bk:
                    btn.configure(image=bK)

                if board.board[j][i] == symbols.wm:
                    btn.configure(image=wM)
                if board.board[j][i] == symbols.wk:
                    btn.configure(image=wK)

            if selected != True:
                btn = Buttons[i][j]
                colour = Colours[j][i]
                if colour == "white":
                    btn.configure(bg="white")

                if colour == "black":
                    btn.configure(bg="black")

            if board.board[j][i] == symbols.w or board.board[j][i] == symbols.b:
                btn = Buttons[i][j]
                btn.configure(image=Empty)
    
    turn = board.turn()
    if turn == "w":
        Turn_var.set("White's turn")
    if turn == "b":
        Turn_var.set("Black's turn")

def click(event):
    global selected, square_selected, piece_selected, Playing, game_over

    try:
        PiecesImagesUpdate()
        grid_info = event.widget.grid_info()

        z = grid_info["column"]
        w = grid_info["row"]

        if 0 <= z <= 8 and 0 <= w <= 8:
            coords = z,w
            Playing = True

    except KeyError or AttributeError:
        Playing = False


    if Playing == True and 0 <= z <= 8 and 0 <= w <= 8 and game_over == False:
        if 0 <= z <= 8 and 0 <= w <= 8:
            if selected == True and (square_selected == coords):
                try:
                    selected = False
                    square_selected = ""
                    piece_selected = None

                except IndexError:
                    selected = True


            elif selected == False:
                try:
                    currentTurn = board.turn()
                    print(board.board[w][z], w, z)
                    if board.board[w][z] != symbols.w or board.board[w][z] != symbols.b:
                        btn = Buttons[z][w]
                        square_selected = z, w
                        piece_selected = w, z
                        if currentTurn == "w":
                            if board.board[square_selected[1]][square_selected[0]][0] in [symbols.wm, symbols.wk]:
                                btn.configure(bg=colour_selected)
                                selected = True

                        if currentTurn == "b":
                            if board.board[square_selected[1]][square_selected[0]][0] in [symbols.bm, symbols.bk]:
                                btn.configure(bg=colour_selected)
                                selected = True

                        if selected != True:
                            square_selected = ""
                            piece_selected = None


                except IndexError:
                    selected = False



            elif selected == True:
                if piece_selected != None:
                    try:
                        pw, pz = piece_selected
                        #print("JEEE ", pw, pz, w, z)
                        if board.is_legal([w, z]):
                            board.make_movement(pw, pz, w, z)
                            board.next_turn()
                            
                        selected = False
                        square_selected = ""
                        piece_selected = None
                            
                        PiecesImagesUpdate()
                        
                    except AttributeError as e:
                        print(e)

            print(selected, piece_selected)
            if selected == True and piece_selected is not None:
                from_coords = [square_selected[1], square_selected[0]]
                possiblemoves = board.legalmoves(from_coords, board)
                #print(possiblemoves)
                
                for i in range(0, len(possiblemoves)):
                    for j in range(0, len(possiblemoves[i])):
                        moves = possiblemoves[i][j]
                        btn = Buttons[moves[1]][moves[0]]
                        btn.configure(bg=colour_possible_moves)


            elif selected == False:
                for i in range(0, 8):
                    for j in range(0, 8):
                        btn = Buttons[i][j]
                        colour = Colours[i][j]

                        if colour == "white":
                            btn.configure(bg="white")

                        if colour == "black":
                            btn.configure(bg="black")


        win = board.check_mate()
        if win != None:
            if win == "b":
                write("Black Wins")
                game_over = True

            if win == "w":
                write("White Wins")
                game_over = True


def loadimages():
    global bM, bK
    global wM, wK, Empty

    # black pieces
    bM = PhotoImage(file="Pieces/bm.gif")
    bK = PhotoImage(file="Pieces/bk.gif")


    # white pieces
    wM = PhotoImage(file="Pieces/wm.gif")
    wK = PhotoImage(file="Pieces/wk.gif")

    Empty = PhotoImage(file="Pieces/Empty.gif")


app = CheckersApp()
app.bind("<Button-1>", click)
app.mainloop()
