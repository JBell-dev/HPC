%
% D.P & O.S for the "HPC Course" at USI and
%                   "HPC Lab for CSE" at ETH Zurich

function [part1,part2] = bisection_inertial(A,xy,picture)
% bisection_inertial : Inertial partition of a graph.
%
% [p1,p2] = bisection_inertial(A,xy) returns a list of the vertices on one side of a partition
%     obtained by bisection with a line or plane normal to a moment of inertia
%     of the vertices, considered as points in Euclidean space.
%     Input A is the adjacency matrix of the mesh (used only for the picture!);
%     each row of xy is the coordinates of a point in d-space.
%
% bisection_inertial(A,xy,1) also draws a picture.


%disp(' ');
%disp(' HPC Lab at USI:   ');
%disp(' Implement inertial bisection');
%disp(' ');


% Steps
% 1. Calculate the center of mass.
n = size(xy,1);
%center of mass as defined in equation 4 of the pdf: 
x_center = (1/n) * sum(xy(:,1));
y_center = (1/n) * sum(xy(:,2));

%display(x_center)
%display(y_center)
% 2. Construct the matrix M.
%  (Consult the pdf of the assignment for the creation of M) 
diff_x = xy(:,1) - x_center;
diff_y = xy(:,2) - y_center;

Sxx = sum(diff_x.^2);
Syy = sum(diff_y.^2);
Sxy = sum(diff_x .* diff_y);

M = [Syy, Sxy; Sxy, Sxx];

%display(M)
% 3. Calculate the smallest eigenvector of M. 
[V,D] = eigs(M, 2, 'smallestreal');
%display(V)
%display(D)

u = V(:,1);
%display(u)
% 4. Find the line L on which the center of mass lies.
%projections = diff_x * u(1) + diff_y * u(2); %as we saw in class (slide 14 - graph)
%since it is implemented in the partition one: 
% 5. Partition the points around the line L.
%   (you may use the function partition.m)
[indices1, indices2] = partition(xy, u);
map = ones(n,1);
map(indices1) = 0;
[part1,part2] = other(map);


if picture == 1
    gplotpart(A,xy,part1);
    title('Inertial bisection using the Fiedler Eigenvector');
end



end
