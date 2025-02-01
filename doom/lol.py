import pygame
import math

pygame.init()

WIDTH, HEIGHT = 800, 600
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("МІЙ ДУМ БЛЯ")

WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
BLUE = (100, 100, 255)

def load_map(filename):
    with open(filename, 'r') as file:
        return [[int(char) for char in line.strip()] for line in file]

MAP = load_map("ебатьмапа/level.catsgoldmap")
MAP_SIZE = len(MAP)
TILE_SIZE = WIDTH // MAP_SIZE // 2

player_pos = [1.5 * TILE_SIZE, 1.5 * TILE_SIZE]
player_angle = 0
player_speed = 3
turn_speed = 0.05

FOV = math.pi / 3
NUM_RAYS = WIDTH // 2
DELTA_ANGLE = FOV / NUM_RAYS
MAX_DEPTH = 800

clock = pygame.time.Clock()

def raycasting():
    start_angle = player_angle - FOV / 2
    for ray in range(NUM_RAYS):
        angle = start_angle + ray * DELTA_ANGLE
        sin_a = math.sin(angle)
        cos_a = math.cos(angle)

        for depth in range(1, MAX_DEPTH):
            x = int(player_pos[0] + cos_a * depth)
            y = int(player_pos[1] + sin_a * depth)
            if MAP[y // TILE_SIZE][x // TILE_SIZE]:
                depth *= math.cos(player_angle - angle)
                wall_height = min(int(HEIGHT / (depth * 0.01)), HEIGHT)

                # Затемнение стены в зависимости от расстояния
                shade_factor = max(0.2, 1 - depth / MAX_DEPTH)
                color = (
                    int(BLUE[0] * shade_factor),
                    int(BLUE[1] * shade_factor),
                    int(BLUE[2] * shade_factor)
                )

                pygame.draw.rect(screen, color, (ray * 2, HEIGHT // 2 - wall_height // 2, 2, wall_height))
                break

def move_player():
    global player_angle
    keys = pygame.key.get_pressed()
    sin_a = math.sin(player_angle)
    cos_a = math.cos(player_angle)

    new_x, new_y = player_pos[0], player_pos[1]
    if keys[pygame.K_w]:
        new_x += cos_a * player_speed
        new_y += sin_a * player_speed
    if keys[pygame.K_s]:
        new_x -= cos_a * player_speed
        new_y -= sin_a * player_speed
    
    if MAP[int(player_pos[1] // TILE_SIZE)][int(new_x // TILE_SIZE)] == 0:
        player_pos[0] = new_x
    if MAP[int(new_y // TILE_SIZE)][int(player_pos[0] // TILE_SIZE)] == 0:
        player_pos[1] = new_y
    
    if keys[pygame.K_a]:
        player_angle -= turn_speed
    if keys[pygame.K_d]:
        player_angle += turn_speed

running = True
while running:
    screen.fill(BLACK)
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    move_player()
    raycasting()
    pygame.display.flip()
    clock.tick(60)

pygame.quit()
