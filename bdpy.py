import RPIO
from RPIO import PWM

"""
Raspberry Pi Port of BallotDiverter V2.
Original code designed by Grant Belton,
Ported to Python by Nam Hee Kim and Brian Tran

NOTE:
    1. "Forward and Reverse" reject method from the
        Arduino version has been deprecated. All rejects will
        use the front reject path.
    2. Rejecting input is now invariable, i.e. pressing the
        experimenter button will always reject paper
        
A very short summary of Raspberry Pi's GPIO control:
    We are going to use the RPIO library for controlling GPIO input and output.
    Using RPIO requires root, so run the python code as root.
    RPIO is useful because it has a built-in servo control support.
    There is a MANDATORY setup procedure. You MUST first set the mode by invoking RPIO.setmode()
    and decide which pin layout we are using. For simplicity's sake, we will be using RPIO.BOARD,
    which is the physical layout. The following describes the aforementioned mandatory setup:
        1. RPIO.setmode() to set the layout scheme
        2. RPIO.setup() to set the pin, using enums RPIO.IN and RPIO.OUT
    And then the rest is similar to inputting and outputting with microcontrollers. RPIO.output() for output pin
    outputs voltage through that pin, and RPIO.input() for input pin reads in a digital signal (True/False)

NOTE on workaround for digital input signals:
    The circuit has to be re-structured to take into account the non-analog input of the sensors.

NOTE on servo:
    The servo motor requires its own power source, lest the board will draw too much current.
    A workaround is to attach an extra Arduino and use its 5v output to power the Raspberry Pi.
"""

# Channel definitions for pins
OptInt1 = 5
OptInt2 = 7
ExpButton = 8
MotorOut = 3
Servo = 11

# constants
optThreshold = 150
servo = PWM.Servo()
positionStraight = 1750
positionDivert = 1250
fullFeed = 1500
frontRejectFeed = 2500

def setup():
    """
    Setting up the GPIO pins for the input output purposes
    """
    print "Running Ballot Diverter V2."

    print "Setting up pins..."

    RPIO.setmode(RPIO.BOARD)

    RPIO.setup(ExpButton, RPIO.IN)
    RPIO.setup(OptInt1, RPIO.IN)
    RPIO.setup(OptInt2, RPIO.IN)
    RPIO.setup(ServoOut, RPIO.OUT)
    RPIO.setup(Motor, RPIO.OUT)

    print "Calibrating the servo..."
    straighten_servo()

def straighten_servo():
    """
    Routine for servo to straighten, so that the paper can go through
    """
    print "Straightening the servo..."
    servo.set_servo(Servo, positionStraight)
    
def divert_servo():
    """
    Routine for servo to move the wings, so that the paper gets diverted
    """
    print "Angling the servo..."
    servo.set_servo(Servo, positionDivert)
    

def front_reject():
    """
    Reject routine that diverts the paper path so that the ballot
    comes out front in accordance with the STAR-Vote specifications.
    """
    # Move the servo
    divert_servo()
    time.sleep(0.015)

    # Rotate the motor
    t0 = int(round(time.time() * 1000))
    t = t0
    while (t - t0 < fullFeed):
        # read digital input from either of the sensors
        t = int(round(time.time() * 1000))
        opt1 = RPIO.input(OptInt1)
        opt2 = RPIO.input(OptInt2)
        if (opt1 or opt2):
            # if paper is still on the way, keep running the loop
            t0 = int(round(time.time() * 1000))
        
        RPIO.output(MotorOut, True)
    
    # fully fed -> stop the motor
    RPIO.output(MotorOut, False)
    time.sleep(1.5) # sleep to ignore spurious input
    
    # straighten servo before jumping into main loop again
    straighten_servo()
    
def accept_ballot():
    """
    Accept routine that runs the motor until the paper is completely put into the box
    """
    
    # move servo
    straighten_servo()
    time.sleep(0.015)
    
    # run motor for fullFeed seconds
    t0 = int(round(time.time() * 1000))
    t = t0
    while (t - t0 < fullFeed):
        # read digital input from either of the sensors
        t = int(round(time.time() * 1000))
        opt1 = RPIO.input(OptInt1)
        opt2 = RPIO.input(OptInt2)
        if (opt1 or opt2):
            # if paper is still on the way, keep running the loop
            t0 = int(round(time.time() * 1000))
        
        RPIO.output(MotorOut, True)
    
    # fully fed -> stop the motor
    RPIO.output(MotorOut, False)
    time.sleep(2) # sleep to ignore spurious input
    
        
def input_loop():
    """
    The main loop for checking the optical interrupters and
    controlling motors
    """

    # read in the values
    while (True):
        opt1 = RPIO.input(OptInt1)
        opt2 = RPIO.input(OptInt2)
        exp = RPIO.input(ExpButton)
        if (opt1 or opt2):
            if (exp):
                front_reject()
            else:
                accept_ballot()
    

    

# Main execution
setup()

while (true):
    input_loop()

