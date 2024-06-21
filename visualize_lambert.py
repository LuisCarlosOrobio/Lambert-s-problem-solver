import numpy as np  # Importing the NumPy library for numerical computations
import matplotlib.pyplot as plt  # Importing Matplotlib for plotting
from mpl_toolkits.mplot3d import Axes3D  # Importing 3D plotting toolkit
from matplotlib.animation import FuncAnimation  # Importing animation functionality

# Read the output from the file
v1 = []  # Initialize an empty list for initial velocity
v2 = []  # Initialize an empty list for final velocity
with open("output.txt", "r") as file:  # Open the output file for reading
    v1 = [float(x) for x in file.readline().strip().split()]  # Read the first line and convert it to a list of floats
    v2 = [float(x) for x in file.readline().strip().split()]  # Read the second line and convert it to a list of floats

# Create vectors for initial and final positions
r1 = np.array([5000, 10000, 2100])  # Initial position vector
r2 = np.array([-14600, 2500, 7000])  # Final position vector
v1 = np.array(v1)  # Convert initial velocity list to a NumPy array
v2 = np.array(v2)  # Convert final velocity list to a NumPy array

# Parameters for the animation
num_frames = 100  # Number of frames in the animation
time_of_flight = 3600  # Total time of flight in seconds
dt = time_of_flight / num_frames  # Time step for each frame

# Create a figure and a 3D axis
fig = plt.figure()  # Create a new figure
ax = fig.add_subplot(111, projection='3d')  # Add a 3D subplot to the figure

# Plot a large sphere for the planet
u, v = np.mgrid[0:2*np.pi:20j, 0:np.pi:10j]  # Create a grid for the sphere
x = 6371 * np.cos(u) * np.sin(v)  # X-coordinates of the sphere surface
y = 6371 * np.sin(u) * np.sin(v)  # Y-coordinates of the sphere surface
z = 6371 * np.cos(v)  # Z-coordinates of the sphere surface
ax.plot_wireframe(x, y, z, color="b", alpha=0.3)  # Plot the sphere as a wireframe

# Plot the initial position of the ship
ship, = ax.plot([], [], [], 'ro')  # Create a red dot for the ship

# Setting up the axis limits
ax.set_xlim([-20000, 20000])  # Set X-axis limits
ax.set_ylim([-20000, 20000])  # Set Y-axis limits
ax.set_zlim([-20000, 20000])  # Set Z-axis limits

# Labels
ax.set_xlabel('X (km)')  # Set X-axis label
ax.set_ylabel('Y (km)')  # Set Y-axis label
ax.set_zlabel('Z (km)')  # Set Z-axis label

# Plot the position vectors for reference
ax.quiver(0, 0, 0, r1[0], r1[1], r1[2], color='b', label='r1')  # Plot initial position vector
ax.quiver(0, 0, 0, r2[0], r2[1], r2[2], color='r', label='r2')  # Plot final position vector
ax.legend()  # Add a legend to the plot

# Function to initialize the animation
def init():
    ship.set_data([], [])  # Initialize the ship data
    ship.set_3d_properties([])  # Initialize the ship 3D properties
    return ship,  # Return the ship object

# Function to update the animation
def update(frame):
    t = frame * dt  # Calculate the current time
    # Calculate position using basic motion equations
    pos = r1 + v1 * t + 0.5 * ((v2 - v1) / time_of_flight) * t**2  # Update position using the velocities
    ship.set_data(pos[0], pos[1])  # Set new data for X and Y coordinates
    ship.set_3d_properties(pos[2])  # Set new data for Z coordinate
    return ship,  # Return the ship object

# Creating the animation
ani = FuncAnimation(fig, update, frames=num_frames, init_func=init, blit=True, interval=100)  # Create the animation

# Display the plot
plt.show()  # Show the plot
