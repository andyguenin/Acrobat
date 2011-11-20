%%
delete(instrfindall);

close all;
clear all;
clc;

%%
% file = serial('COM4', 'Baudrate', 9600);
% disp('Connected to MAEVARM');
% fopen(file);
% disp('Opened MAEVARM');
% input = zeros(1,4);
% in = fscanf(file);
% 
% while(strcmp('quit\n', in) == 0)
%    
%     disp(in);
%     in =  fscanf(file);
% end
% fclose(s);
% disp('closed');


file = serial('COM4', 'Baudrate', 9600);
disp('Connected to MAEVARM');
fopen(file);
disp('Opened MAEVARM');
in = fread(file, 16);
angle = [];
velocity = [];
pwm = [];

p = [];
i = [];
d = [];

maxshow = 300;

while(1)
    if(length(in) == 16)
        if(in(1) == 'a')
            a = str2double(char(in(2:6)'));
            v = str2double(char(in(7:11)'));
            pw = str2double(char(in(12:16)'));

            angle = [angle a];
            velocity = [velocity v];
            pwm = [pwm pw];

            if(length(angle) > maxshow)
                angle = angle(2:maxshow+1);
                velocity = velocity(2:maxshow+1);
                pwm = pwm(2:maxshow+1);
            end
            subplot(3,3,[2 3]);
            plot(angle)
            title('Angle')
            axis([0 maxshow 0 1024]);
            subplot(3,3,[5 6]);
            plot(velocity)
            title('Velocity')
            axis([0 maxshow 0 1024]);
            subplot(3,3,[1 4]);
            plot(pwm)
            title('PWM')
            axis([0 maxshow 0 1024]);
            drawnow;
        end
        if(in(1) == 'p')
            inp = str2double(char(in(2:6)'));
            ini = str2double(char(in(7:11)'));
            ind = str2double(char(in(12:16)'));

            p = [p inp];
            i = [i ini];
            d = [d ind];

            if(length(i) > maxshow)
                p = p(2:maxshow+1);
                i = i(2:maxshow+1);
                d = d(2:maxshow+1);
            end

            subplot(3,3,7);
            plot(p)
            title('P')
            axis([0 maxshow 0 1024]);
            subplot(3,3,8);
            plot(i)
            title('I')
            axis([0 maxshow 0 1024]);
            subplot(3,3,9);
            plot(d)
            title('D')
            axis([0 maxshow 0 1024]);
            drawnow;

          %  disp('-----------------------------')
          %  disp(strcat('P: ', num2str(p)));
          %  disp(strcat('I: ', num2str(i)));
          %  disp(strcat('D: ', num2str(d)));
        end
        in = fread(file, 16);
    end
end