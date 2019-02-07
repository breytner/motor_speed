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
t = 0:0.005:2;
% y = step(sysb, t);
% figure(1);
% plot(t,y);
% xlabel("Time (s)");
% ylabel("Motor speed (V)");
% legend("Open loop unit step response");

figure(1);
trace = load("trace.dat");
plot(trace(:,1), trace(:,2), '-r');
legend("Motor speed (SystemC)", "Motor speed (Octave)");
hold on;

%% Close loop system response
C = pid(3, 15, 0.3)
S = C*sysb;

s = tf('s');
kp = 3
ki = 15
C2 = kp + ki/s + 0.3*s/(1+0.001*s);
S = C2*sysb;
U = C2 / (1 + S);

T = feedback(C*sysb,1)
y_c = step(T,t);
plot(t, y_c);

u_c = step(U,t); 
figure(2);
plot(t, u_c);
hold on;
stairs(trace(:,1), trace(:,3), '-r');
legend("Control signal (Octave)", "Control signal (SystemC)");

disp ("The maximum value in trace is:"); 
max_sc = max(trace(:,2)); 
disp(max_sc);
disp ("The maximum value in octaves sim is:"); 
max_oc = max(y_c); 
disp(max_oc);
disp ("The error is:"); 
disp(abs(max_oc - max_sc));


input ("Press enter to exit");