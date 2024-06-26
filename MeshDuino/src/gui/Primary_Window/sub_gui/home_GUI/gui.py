from tkinter import PhotoImage, Canvas, Entry, Button, Toplevel, Label, messagebox, Tk, LEFT
import threading
from pathlib import Path
from PIL import Image, ImageTk  # Import Pillow

# Define the relative_to_assets function
def relative_to_assets(path: str) -> Path:
    return Path(__file__).parent / "assets" / Path(path)

# Define the Home function
def Home(parent):
    global arduino_img  # Make the variable global
    canvas = Canvas(
        parent,
        bg="#FFFFFF",
        height=405,
        width=675,
        bd=0,
        highlightthickness=0,
        relief="ridge"
    )

    canvas.place(x=230, y=72)

    canvas.create_text(
        150.0,
        58.0,
        anchor="nw",
        text="Welcome to Meshduino!",
        fill="#C67FFC",
        font=("Montserrat Bold", 32 * -1)
    )
    canvas.create_text(
        150.0,
        100.0,
        anchor="nw",
        text="An Integrated Arduino and Python Network",
        fill="#C67FFC",
        font=("Montserrat Bold", 18 * -1)
    )
    
    # Adding Arduino image
    arduino_image_path = relative_to_assets("arduino_uno_1.png")
    arduino_img = Image.open(arduino_image_path)
    arduino_img = arduino_img.resize((200, 200), Image.LANCZOS)  # Resize the image
    arduino_img = ImageTk.PhotoImage(arduino_img)
    canvas.create_image(200, 150, image=arduino_img, anchor="nw")

    # Help button
    global help_button_image
    help_button_image = PhotoImage(file=relative_to_assets("help_button.png"))
    Button(
        image=help_button_image,
        borderwidth=0,
        highlightthickness=0,
        command=show_help,
        relief="flat",
        bg='#FFFFFF',
        activebackground='#FFFFFF'
    ).place(
        x=680.0,
        y=25.0,
        width=200.0,
        height=55.0
    )

def show_help():
    help_window = Toplevel()
    help_window.title("Help")
    help_window.geometry("700x500")
    help_window.configure(bg="#FFFFFF")
    help_window.iconbitmap(relative_to_assets("minilogo.ico"))

    Label(
        help_window,
        text="Help and Support",
        font=("Montserrat Bold", 24),
        fg="#171435",
        bg="#FFFFFF"
    ).pack(pady=10)

    help_text = """
    Welcome to Meshduino!

    Meshduino is an Arduino network integrated with Python, 
    featuring a GUI.

    Sections:
    - Home: Introduction to Meshduino and its features.
    - Challenges: View and participate in available challenges.
    - Leaderboard: See the best times and rankings of teams.
    - About Us: Learn more about our team.

    For further assistance, please contact us at:
    meshduino@gmail.com

    Future Products and Tutorials:
    - Detailed guides for Users/Parents/Teachers.
    - Comprehensive tutorials for Students to master challenges.
    """

    Label(
        help_window,
        text=help_text,
        font=("Montserrat", 14),
        fg="#171435",
        bg="#FFFFFF",
        justify=LEFT,
        wraplength=580
    ).pack(pady=10, padx=20)

