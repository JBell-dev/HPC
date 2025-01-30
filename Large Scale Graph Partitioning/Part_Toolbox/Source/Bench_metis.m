function [cut_recursive,cut_kway] = Bench_metis(picture)
% Compare recursive bisection and direct k-way partitioning,
% as implemented in the Metis 5.0.2 library.

%  Add necessary paths
addpaths_GP;

% Graphs in question
%load usroads;
%load luxembourg_osm;
%disp(fieldnames(Problem));

%A = Problem.A; %when the matrix is loaded, it is stored a struct "Problem" that contains A .
%display(size(A));
%xy = Problem.aux.coord;  % after searching for it, they were in .aux

%FOR THE COUNTRIES ONE - i use what we develop in first exercise: 
%country = "RU-40527";
%base_path = '/Users/jonatan/Documents/MAI/Y24/S1/HPC/TP6/Part_Toolbox';
%csv_path = fullfile(base_path, 'Datasets/Countries_Meshes/csv');
%adj_file = fullfile(csv_path, sprintf('%s-adj.csv', country)); 
%pts_file = fullfile(csv_path, sprintf('%s-pts.csv', country));
%edges = readmatrix(adj_file); %we get the edges  -> out to in  (2 col)
%coords = readmatrix(pts_file); %we get the coords -> (x,y)
%n = size(coords, 1); %take the length of the coords
%https://ch.mathworks.com/help/matlab/ref/sparse.html#d126e1665696
%adj_matrix = sparse(edges(:,1), edges(:,2), 1, n, n); %this stores "out to in"


%A = adj_matrix + adj_matrix'; %now out to in is added by transpose - so now it is symmetric
%xy = coords;

%SWISS CASE:
load Swiss_graph.mat
A = CH_adj + CH_adj';
A = sparse(A);
xy = CH_coords;
% Steps

% 1. Initialize the cases
partitions = [16, 32];
recursive = zeros(size(partitions));
kway = zeros(size(partitions));

for p = 1:length(partitions)
    nparts = partitions(p);
    %display(nparts);

    % 2. Call metismex to
    %     a) Recursively partition the graphs in 16 and 32 subsets.
    [part_recursive, edgecut_recursive] = metismex('PartGraphRecursive', A, nparts); %file: METIS_PartGraphRecursive_mex
    %returns partition assignment, number of edges that cross between
    %partitions. 
    cut_recursive(p) = edgecut_recursive;
    fprintf('Recursive cut: %d\n', edgecut_recursive);
    %display(part_recursive);

    %b) Perform direct k-way partitioning of the graphs in 16 and 32 subsets.
    [part_kway, edgecut_kway] = metismex('PartGraphKway', A, nparts);
    cut_kway(p) = edgecut_kway;
    fprintf('K-way cut: %d\n', edgecut_kway);

    % 3. Visualize the results for 32 partitions.
    if nparts == 32
        % recursive bisection 
        figure;
        gplotg(A, xy, 'k-');  
        hold on;
        scatter(xy(:,1), xy(:,2), 50, part_recursive, 'filled');
        title(sprintf('Recursive Bisection - 32 partitions - "Switzerland" - \nEdge Cut: %d', edgecut_recursive));
        colorbar;
        
        % k-way partitioning 
        figure;
        gplotg(A, xy, 'k-');  
        hold on;
        scatter(xy(:,1), xy(:,2), 50, part_kway, 'filled');
        title(sprintf('Direct K-way - 32 partitions - "Switzerland" - \nEdge Cut: %d', edgecut_kway));
        colorbar;
    end

end