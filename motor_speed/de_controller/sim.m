pkg load control;

jm = 1.13e-2;
b = 0.028;
la = 1e-1;
ra = 0.45;
kt = 0.067;
ke = kt;

%% Open loop system response
numb = [ kt/(jm*la)];
denb = [1 (ra/la + b/jm) (b*ra+kt*ke)/(jm*la)];
sysb = tf(numb, denb);

%% Close loop system response
C = pid(3, 15, 0.3)
S = C*sysb;

T = feedback(S, 1);
t = [0:0.01:1.4];
y_c = step(T,t);

figure(2);
plot(t, y_c)
hold on;
trace = load("trace.dat");
plot(trace(20:20:end,1), trace(20:20:end,2), 'ro', 'Linewidth', 2);
hold on;
xlabel("Time (s)");
ylabel("Motor speed (V)");
legend("Octave simulation approx. derivative", "DE controlled speed");

input ("Press enter to exit");