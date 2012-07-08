function DOIT()
    for i=0:11
        a = fopen(['training_' int2str(i) '.txt'], 'w');
    end
    
    fds = fopen('all');
    
    arr = {'C', 'CS', 'D', 'DS', 'E', 'F', 'FS', 'G', 'GS', 'A', 'AS', 'B'};
   
    for k=0:39
        [f,t] = wavread(['wav/' int2str(k) '.wav']);
        S = myspecgram(f(1770:2770),256,t);
        [m,n] = size(S);
        for o=1:12
            out = fopen(['label/' int2str(k) '_' char(arr(o)) '.label']);
            label = fscanf(out, '%d');
            fprintf(fds(o), '%d ', label);
            for i=1:m
                for j=1:n
                    fprintf(fds(o), '%d:%f ', (i-1)*n+j, S(i,j));
                end
            end
            fprintf(fds(o),'\n');
        end
    end
    
    fclose('all');
end

