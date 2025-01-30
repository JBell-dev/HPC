function [part1,part2] = bisection_spectral(A,coords,picture)
% bisection_spectral : Spectral partition of a graph.
%
% D.P & O.S for the "HPC Course" at USI and
%                   "HPC Lab for CSE" at ETH Zuric
%
% [part1,part2] = bisection_spectral(A) returns a partition of the n vertices
%                 of A into two lists part1 and part2 according to the
%                 spectral bisection algorithm of Simon et al:
%                 Label the vertices with the components of the Fiedler vector
%                 (the second eigenvector of the Laplacian matrix) and partition
%                 them around the median value or 0.
    
    
    % Steps
    % 1. Construct the Laplacian. -> L = D - A
    % given A we need to construct D:
    D = diag(sum(A,2));
    L = D - A; %get the laplacian as we saw in class
    %display(L)
    
    % 2. Calculate its eigensdecomposition.
    [V,D] = eigs(L, 2, 'smallestreal');  % https://ch.mathworks.com/help/matlab/ref/eig.html -> then by running it get an error that suggest -> eigs and need to reader since it returns the highest first.
    
    % 3. Label the vertices with the components of the Fiedler vector.
    fiedler = V(:,2); %as we saw in class, we take the second highest eigenvector. 

    % 4. Partition them around their median value, or 0. 
    map = zeros(length(fiedler),1); %it actually provided better results in the case of vietnam so i keep this one. -> 11 cuts vs 15 
    map(fiedler > 0) = 1;

    %med = median(fiedler); 
    %map = zeros(length(fiedler),1);
    %map(fiedler > med) = 1;
    
    [part1,part2] = other(map);
    
    if picture == 1
        gplotpart(A,coords,part1);
        title('Spectral bisection using the Fiedler Eigenvector');
    end

end

