import matplotlib.pyplot as plt
from math import sqrt

Li = 0
Lh = 47e-6
Cout = 100e-6
Vin = 3
Vout = Vin + 1
Iload = 0

data = []

class Regulator:
    def __init__(self, setpoint):
        self.period = 12e-6
        self.ton = 0
        self.toff = self.period
        self.duty = 0.0
        self.vset = setpoint
        self.vout = 0

    def update(self, Vin, Vout, Iload):
        self.Li = (Vout - self.vout) * Cout / self.toff + Iload 
        self.vout = Vout
        vdif = self.vset - self.vout
        if vdif > 0:
            coulombs_needed = vdif * Cout
            amps_needed = sqrt(coulombs_needed / Lh * (vdif/2+Vin))
            amps_needed -= self.Li
        else:
            amps_needed = Iload
        if Vout > Vin:
            self.duty = amps_needed * Lh * (1-Vin/Vout) / self.period
            max_duty = 1 - 0.5*Vin/Vout
            if self.duty>max_duty:
                self.duty = max_duty
        else:
            self.duty = 0
        self.ton = self.period * self.duty
        self.toff = self.period * (1-self.duty)
        return self.ton, self.toff
    def setpoint(self, setpoint):
        self.vset = setpoint

class Charger():
    def __init__(self):
        self.speed = 0
        self.watts = [0, 5, 10, 15]
        self.amps = 0
    def reqv(self):
        if self.speed > 2:
            return 9
        else:
            return 5
    def update(self, Vout):
        match self.speed:
            case 0: # Not charging
                if Vout > 4.5 and Vout < 5.5:
                    self.speed = 1
            case 1: # Slow charging
                if Vout > 5.5 or Vout < 4.5:
                    self.speed = 0
                elif self.amps > 0.4:
                    self.speed = 2
            case 2: # Normal charging
                if Vout > 6 or Vout < 4:
                    self.speed = 0
                elif self.amps > 0.8 and Vout >= 5:
                    self.speed = 2 #Disable fast-charging for now
            case 3: # Fast charging
                if Vout > 9 or Vout < 4.8:
                    self.speed = 1
        if self.speed > 0:
            self.amps = self.watts[self.speed] / Vout
        else:
            self.amps = 0.1
        return self.amps

u1 = Regulator(5.0)
u2 = Charger()

for j in range(int(5e3)):
    Iload = u2.update(Vout)
    if j%10 == 0:
        u1.setpoint(u2.reqv())
    Ton,Toff = u1.update(Vin,Vout,Iload)
    Li += Vin * Ton / Lh
    di = min((Vout - Vin) * Toff / Lh, Li)
    Vout += Toff * (Li - di/2) / Cout
    Vout -= (Ton + Toff) * Iload / Cout
    Li -= di
    data.append((Vout,u1.duty,Iload,Li,u1.Li,u2.speed))

for d in zip(*data):
    plt.plot(list(d))
plt.legend(['Vout','Duty','Iload','Li','u1.Li','speed'])
plt.show()

