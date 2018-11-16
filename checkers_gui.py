import tkinter as tk
from tkinter import *
from tkinter import ttk, filedialog, messagebox, colorchooser

global colour_selected, colour_possible_moves
colour_selected = "orange"
colour_possible_moves = "orange"

LARGE_FONT = ("Verdana", 40)


class Board(object):
    # The chess board is represented as a 8x8 2D array
    def __init__(self):
        self._turn = "w"
        
        self.board = [
            ["", "bm", "", "bm", "", "bm", "", "bm"],
            ["bm", "", "bm", "", "bm", "", "bm", ""],
            ["", "bm", "", "bm", "", "bm", "", "bm"],
            ["", "", "", "", "", "", "", ""],
            ["", "", "", "", "", "", "", ""],
            ["wm", "", "wm", "", "wm", "", "wm", ""],
            ["", "wm", "", "wm", "", "wm", "", "wm"],
            ["wm", "", "wm", "", "wm", "", "wm", ""]
            ]


    def legalmoves(self, from_coords, board):
        # print(from_coords)
        piece = self.__board[from_coords[0]][from_coords[1]]
        return piece.get_legal_moves(from_coords, self.__board)

    def turn(self):
        return self._turn
        
        
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

                if board[y][x] != "":
                    if board[y][x] == "bm":
                        btn.configure(image=bM)
                    if board[y][x] == "bk":
                        btn.configure(image=bK)

                    if board[y][x] == "wm":
                        btn.configure(image=wM)
                    if board[y][x] == "wk":
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
            if board[j][i] != "":
                if board[j][i] == "bm":
                    btn.configure(image=bM)
                if board[j][i] == "bk":
                    btn.configure(image=bK)

                if board[j][i] == "wm":
                    btn.configure(image=wM)
                if board[j][i] == "wk":
                    btn.configure(image=wK)
                

            if selected != True:
                btn = Buttons[i][j]
                colour = Colours[j][i]
                if colour == "white":
                    btn.configure(bg="white")

                if colour == "black":
                    btn.configure(bg="black")

            if board[j][i] == "":
                btn = Buttons[i][j]
                btn.configure(image=Empty)
    
    turn = "W"
    if turn == "W":
        Turn_var.set("White's turn")
    if turn == "B":
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
                    piece_selected = ""

                except IndexError:
                    selected = True


            elif selected == False:
                try:
                    currentTurn = "W"
                    if board[w][z] != "":
                        btn = Buttons[z][w]
                        square_selected = z, w
                        piece_selected = board[w][z]
                        if currentTurn == "W":
                            if board[square_selected[1]][square_selected[0]][0] == "w":
                                btn.configure(bg=colour_selected)
                                selected = True

                        if currentTurn == "B":
                            if board[square_selected[1]][square_selected[0]][0] == "b":
                                btn.configure(bg=colour_selected)
                                selected = True

                        if selected != True:
                            square_selected = ""
                            piece_selected = ""


                except IndexError:
                    selected = False



            elif selected == True:
                if piece_selected != "":
                    try:
                        selected = Movement(square_selected,coords,piece_selected)
                        if selected == False:
                            #board.Next_Turn()
                            square_selected = ""
                            piece_selected = ""
                        PiecesImagesUpdate()
                        
                    except AttributeError:
                        selected = True

            if selected == True and piece_selected is not None:
                from_coords = [square_selected[1], square_selected[0]]
                possiblemoves = board.legalmoves(from_coords, board)
                #print(possiblemoves)

                from_coords = [square_selected[1], square_selected[0]]
                possiblemoves = board.legalmoves(from_coords, board)

                for i in range(0, len(possiblemoves)):
                    moves = possiblemoves[i]
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
            if win[0] == "B":
                write("Black Wins")
                coords = win[1]
                button = Buttons[coords[1]][coords[0]]
                button.configure(bg="red")
                game_over = True

            if win[0] == "W":
                write("White Wins")
                coords = win[1]
                button = Buttons[coords[1]][coords[0]]
                button.configure(bg="red")
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
