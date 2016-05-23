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
"""

# Channel definitions for pins

# TODO
ExpButton = 8
OptInt1 = 5
OptInt2 = 7
MotorOut = 3
Servo = 11

optThreshold = 150
servo = PWM.Servo()
positionStraight = 132
positionDivert = 109
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
    servo.set_servo(17, 1200)


def front_reject():
    """
    Reject routine that diverts the paper path so that the ballot
    comes out front in accordance with the STAR-Vote specifications.
    """

    # Move the servo

    # Rotate the motor

def input_loop():
    """
    The main loop for checking the optical interrupters and
    controlling motors
    """

    # read in the values
    while (True):
        opt1_val = RPIO.input(OptInt1)
        opt2_val = RPIO.input(OptInt2)
        exp_val = RPIO.input(ExpButton)
        if (opt1_val or opt2_val):
            if (ExpButton):
                front_reject()
            else:
                accept_ballot()
    

    

# Main execution
setup()

while (true):
    input_loop()

