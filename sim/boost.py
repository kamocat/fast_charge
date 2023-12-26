import matplotlib.pyplot as plt


Li = 0
Lh = 470e-6
Cout = 100e-6
Vin = 4
Vout = 0

Vout_plot = []
Li_plot = []

class Regulator:
    def __init__(self, setpoint):
        self.period = 10e-6
        self.duty = 0.0
        self.vset = setpoint

    def update(self, Vout):
        if Vout < self.vset:
            self.duty += 0.01
        else:
            self.duty -= 0.01
        if self.duty>0.8:
            self.duty = 0.8
        elif self.duty<0:
            self.duty = 0
        return (self.duty    *self.period,
                (1-self.duty)*self.period)

u1 = Regulator(5.0)

for j in range(int(5e3)):
    Ton,Toff = u1.update(Vout)
    Iload = 0.1
    Li += Vin * Ton / Lh
    di = min((Vout - Vin) * Toff / Lh, Li)
    Vout += Toff * (Li - di/2) / Cout
    Vout -= (Ton + Toff) * Iload / Cout
    Li -= di
    Vout_plot.append(Vout)
    Li_plot.append(Li)

print(Vout)
print(Li)
plt.plot(Vout_plot)
plt.plot(Li_plot)
plt.show()

