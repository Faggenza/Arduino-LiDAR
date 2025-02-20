import tkinter as tk
import serial
import serial.tools.list_ports
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt
import numpy as np

class DataVisualizer:
    def __init__(self, root):
        self.root = root
        self.root.title("LiDAR Data Visualizer")
        
        self.figure, self.ax = plt.subplots(subplot_kw={'projection': 'polar'})
        self.ax.set_ylim(0, 1000)
        self.canvas = FigureCanvasTkAgg(self.figure, master=root)
        self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)
        
        self.serial_port = serial.Serial('/dev/ttyUSB0', 230400, timeout=None)
        
        self.canvas.mpl_connect("motion_notify_event", self.on_hover)
        
        self.annotation = self.ax.annotate("", xy=(0,0), xytext=(-55,20),
                                           textcoords="offset points",
                                           bbox=dict(boxstyle="round", fc="w"),
                                           arrowprops=dict(arrowstyle="->"))
        self.annotation.set_visible(False)
        
        self.scatter = self.ax.scatter([], [], s=4)
        self.data_vect = np.full(36000, np.nan)
        
        self.update_plot()
        
        self.scrollbar = tk.Scale(root, from_=500, to=8000, orient=tk.HORIZONTAL, label="Y-axis Limit", command=self.update_ylim)
        self.scrollbar.pack(fill=tk.X)
    
    def update_plot(self):
        data = self.read_serial_data()
        if data:
            for angle, distance in data:
                index = int(angle*100)
                if 0 <= index < 36000:
                    self.data_vect[index] = distance

            valid_indices = ~np.isnan(self.data_vect) & (self.data_vect >= 100)
            angles = np.radians(np.arange(36000)[valid_indices] / 100)
            distances = self.data_vect[valid_indices]
            
            self.scatter.set_offsets(np.c_[angles, distances])
            self.canvas.draw()
        
        self.root.after(1, self.update_plot)
    
    def read_serial_data(self):
        line = self.serial_port.readline().decode('utf-8').strip()
        if line and line.startswith('A'):      
            try:
                data = line[2:].split(',')
                formatted_data = [(float(data[i]), float(data[i+1])) for i in range(0, len(data), 2)]
                return formatted_data
            except Exception as e:
                print(f"Error parsing data: {e}")
                return None
            
        return None
    
    def on_hover(self, event):
        if event.inaxes == self.ax and self.scatter:
            cont, ind = self.scatter.contains(event)
            if cont:
                idx = ind["ind"][0]
                angle = self.scatter.get_offsets()[idx][0]
                distance = self.scatter.get_offsets()[idx][1]
                self.annotation.xy = (angle, distance)
                self.annotation.set_text(f"Angle: {np.degrees(angle):.2f}°\nDistance: {distance:.2f} mm")
                self.annotation.set_visible(True)
                self.canvas.draw_idle()
            else:
                self.annotation.set_visible(False)
                self.canvas.draw_idle()
    
    def update_ylim(self, value):
        self.ax.set_ylim(0, int(value))
        self.canvas.draw()

if __name__ == "__main__":
    root = tk.Tk()
    app = DataVisualizer(root)
    root.mainloop()
