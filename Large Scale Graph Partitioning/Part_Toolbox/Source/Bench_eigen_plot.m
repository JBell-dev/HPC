% Visualize information from the eigenspectrum of the graph Laplacian
%
% D.P & O.S for the "HPC Course" at USI and
%                   "HPC Lab for CSE" at ETH Zurich

% add necessary paths
addpaths_GP;

% Graphical output at bisection level
picture = 0;

% Cases under consideration
% load airfoil1.mat;
% load 3elt.mat;
% load barth4.mat;
% load mesh3e1.mat;
load crack.mat;

% Initialize the cases
W      = Problem.A;
coords = Problem.aux.coord;

% Steps
% 1. Construct the graph Laplacian of the graph in question.
D = diag(sum(W));
L = D - W;
% 2. Compute eigenvectors associated with the smallest eigenvalues.
[V,D] = eigs(L,3,"smallestabs"); 
display(D);
first = V(:,1);
sec = V(:,2);
third = V(:,3);
% 3. Perform spectral bisection.
map = zeros(length(sec),1); 
map(sec > 0) = 1;
%display(map);
% 4. Visualize:
%   i.   The first and second eigenvectors.
figure('Name', 'First Two Eigenvectors'); 
subplot(2,1,1); 
plot(V(:,1), 'b.-'); 
title('First Eigenvector'); 
xlabel('Node Index'); 
ylabel('Eigenvector Value'); 
grid on; 

subplot(2,1,2); 
plot(V(:,2), 'r.-'); 
title('Second Eigenvector (Fiedler)'); 
xlabel('Node Index'); 
ylabel('Eigenvector Value'); 
grid on;
%   ii.  The second eigenvector projected on the coordinate system space of graphs.
%bisection
part1 = find(sec < 0);
part2 = find(sec >= 0);
%i scale up the z axis of the eigenvalues since sometimes it is hard to see
%the change
coord_range = max(max(coords) - min(coords));
eig_range = max(sec) - min(sec);
scale_factor = coord_range / eig_range;
sec_scaled = sec * scale_factor;
figure('Name', 'Fiedler projection based on partitions and scatter of its eigenvectors values');
set(gcf, 'Color', 'white'); 

% partitions at z=0 plane using original 2D coords
% one partition in black, the other in gray, and cut edges in red
gplotpart(W, coords, part1, 'k', [0.7 0.7 0.7], 'r');
hold on;
% overlay the eigenvector values in 3D above the plane
scatter3(coords(:,1), coords(:,2), sec_scaled, 30, sec, 'filled');
colormap(jet);
colorbar;
view(45,30);
set(gca, 'Color', 'white');
title('Fiedler projection based on partitions and scatter of its eigenvectors values');
xlabel('X');
ylabel('Y');
zlabel('Second Eigenvector Value (v_2)');
axis tight; 
grid on;
%   iii. The spectral bi-partitioning results using the spectral coordinates of each graph.
coords_spectral = [sec, third];

figure('Name', 'Spectral Coordinates Bipartitioning');
set(gcf, 'Color', 'white'); 

% Plot partitions in the spectral coordinate space
coords_spectral = [sec, third];
gplotpart(W, coords_spectral, part1, 'k', [0.7 0.7 0.7], 'r');
hold on;
title('Bipartitioning using spectral coordinates (v_2 and v_3)');
xlabel('v_2');
ylabel('v_3');
axis equal;  
grid on;

