function varargout = acrobat(varargin)
% ACROBAT M-file for acrobat.fig
%      ACROBAT, by itself, creates a new ACROBAT or raises the existing
%      singleton*.
%
%      H = ACROBAT returns the handle to a new ACROBAT or the handle to
%      the existing singleton*.
%
%      ACROBAT('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in ACROBAT.M with the given input arguments.
%
%      ACROBAT('Property','Value',...) creates a new ACROBAT or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before acrobat_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to acrobat_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help acrobat

% Last Modified by GUIDE v2.5 31-Oct-2011 17:26:19



% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @acrobat_OpeningFcn, ...
                   'gui_OutputFcn',  @acrobat_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end



if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end




% End initialization code - DO NOT EDIT


% --- Executes just before acrobat is made visible.
function acrobat_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to acrobat (see VARARGIN)
    delete(instrfindall);
    
    global angle;
    global velocity;
    global PWM;
    global maxlength;

    maxlength = 1000;

    angle = [];
    velocity = [];
    PWM = [];

    file = serial('COM4', 'Baudrate', 9600);
    fopen(file);
    %handles.status_text.string = 'working';
    set(handles.status_text, 'String', 'working');
    
    t = timer('TimerFcn', 'acquire_data()', 'StartDelay',10, 'ExecutionMode', 'fixedDelay', 'Period', .001);
             
    start(t);
    


% Choose default command line output for acrobat
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes acrobat wait for user response (see UIRESUME)
% uiwait(handles.figure1);

% --- Outputs from this function are returned to the command line.
function varargout = acrobat_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


function acquire_data()
    global angle;
    global velocity;
    global PWM;
    global maxlength;
    in = fread(file, 16);
    if(in(1) == 'a')
        in_a = str2double(char(in(2:6)'));
        in_v = str2double(char(in(7:11)'));
        in_PWM = str2double(char(in(12:16)'));
    end
    
    angle = [angle in_a];
    if(length(angle) > maxlength)
        angle = angle(2:maxlength+1);
    end
    
    velocity = [velocity in_v];
    if(length(velocity) > maxlength)
        velocity = velocity(2:maxlength+1);
    end

    PWM = [PWM in_PWM];
    if(length(PWM) > maxlength)
        PWM = PWM(2:maxlength+1);
    end
    
    plot(handles.axes1, angle);
    


function p_value_Callback(hObject, eventdata, handles)
% hObject    handle to p_value (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of p_value as text
%        str2double(get(hObject,'String')) returns contents of p_value as a double


% --- Executes during object creation, after setting all properties.
function p_value_CreateFcn(hObject, eventdata, handles)
% hObject    handle to p_value (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function i_value_Callback(hObject, eventdata, handles)
% hObject    handle to i_value (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of i_value as text
%        str2double(get(hObject,'String')) returns contents of i_value as a double


% --- Executes during object creation, after setting all properties.
function i_value_CreateFcn(hObject, eventdata, handles)
% hObject    handle to i_value (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function d_value_Callback(hObject, eventdata, handles)
% hObject    handle to d_value (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of d_value as text
%        str2double(get(hObject,'String')) returns contents of d_value as a double


% --- Executes during object creation, after setting all properties.
function d_value_CreateFcn(hObject, eventdata, handles)
% hObject    handle to d_value (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in pushbutton1.
function pushbutton1_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
