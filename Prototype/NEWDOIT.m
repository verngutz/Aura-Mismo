function NEWDOIT(parameter)
    a = fopen('testing.txt', 'w');
   
    [f,t] = wavread(parameter);
    [r,w] = size(f);
    for i=0:r/1000
        S = myspecgram(f(i*1000 + 1:(i+1)*1000 + 1),256,t);
        [m,n] = size(S);    
    
        for z=1:m
            for j=1:n
                fprintf(a, '%d:%f ', (z-1)*n+j, S(z,j));
            end
        end
    
        fprintf(a, '\n');
    end
    fclose('all');
end

