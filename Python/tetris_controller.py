
import time
import pygame
import serial
from serial.tools import list_ports

BAUD = 115200
REPEAT_DELAY = 0.12


def choose_port():
    ports = list(list_ports.comports())

    if not ports:
        print("Arduino not found. Check USB cable.")
        return None

    print("\nAvailable COM Ports:")
    for i, port in enumerate(ports, 1):
        print(f"{i}. {port.device} - {port.description}")

    while True:
        choice = input("\nChoose a number of PORT or write COM port: ").strip()

        if choice.upper().startswith("COM"):
            return choice.upper()

        try:
            index = int(choice) - 1
            if 0 <= index < len(ports):
                return ports[index].device
        except ValueError:
            pass

        print("Incorrect option. Try again.")


def main():
    port = choose_port()
    if port is None:
        input("Press enter to exit.")
        return

    try:
        arduino = serial.Serial(port, BAUD, timeout=0.01)
    except serial.SerialException as error:
        print(f"Cannot open {port}: {error}")
        print("Close Arduino Serial Monitor and try again.")
        input("Press enter to leave")
        return

    
    time.sleep(2)
    arduino.reset_input_buffer()

    def send(command):
        try:
            arduino.write(command.encode("ascii"))
        except serial.SerialException:
            print("USB connection has stopped.")

    pygame.init()
    screen = pygame.display.set_mode((520, 300))
    pygame.display.set_caption("Tetris - PC Controller")
    clock = pygame.time.Clock()

    title_font = pygame.font.SysFont("consolas", 25)
    font = pygame.font.SysFont("consolas", 18)

    running = True
    left_held = False
    right_held = False
    last_left = 0.0
    last_right = 0.0

    print("\nController has stopped!")
    print("A/Left = Left | D/Right = Right")
    print("W/Up = Rotate | S/Down = Slower faster fall")
    print("Space = Instant drop | R = restart after Game Over")

    while running:
        now = time.monotonic()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

            elif event.type == pygame.KEYDOWN:
                if event.key in (pygame.K_a, pygame.K_LEFT):
                    send("L")
                    left_held = True
                    last_left = now

                elif event.key in (pygame.K_d, pygame.K_RIGHT):
                    send("D")
                    right_held = True
                    last_right = now

                elif event.key in (pygame.K_w, pygame.K_UP):
                    send("W")

                elif event.key in (pygame.K_s, pygame.K_DOWN):
                    send("S")

                elif event.key == pygame.K_SPACE:
                    send("F")

                elif event.key == pygame.K_r:
                    send("R")

            elif event.type == pygame.KEYUP:
                if event.key in (pygame.K_a, pygame.K_LEFT):
                    left_held = False

                elif event.key in (pygame.K_d, pygame.K_RIGHT):
                    right_held = False

                elif event.key in (pygame.K_s, pygame.K_DOWN):
                    send("s")

    
        if left_held and now - last_left >= REPEAT_DELAY:
            send("L")
            last_left = now

        if right_held and now - last_right >= REPEAT_DELAY:
            send("D")
            last_right = now

        screen.fill((25, 28, 35))

        lines = [
            ("TETRIS CONTROLLER", title_font),
            ("A / LEFT ARROW   - Move left", font),
            ("D / RIGHT ARROW  - Move right", font),
            ("W / UP ARROW     - Rotate", font),
            ("S / DOWN ARROW   - Soft drop (hold)", font),
            ("SPACE            - Hard drop", font),
            ("R                - Restart after Game Over", font),
            (f"Connected to {port} at {BAUD} baud", font),
            ("Close window to quit", font),
        ]

        y = 20
        for text, selected_font in lines:
            image = selected_font.render(text, True, (240, 240, 240))
            screen.blit(image, (20, y))
            y += 32 if selected_font == title_font else 29

        pygame.display.flip()
        clock.tick(120)

    try:
        send("s")
        arduino.close()
    except serial.SerialException:
        pass

    pygame.quit()


if __name__ == "__main__":
    main()
