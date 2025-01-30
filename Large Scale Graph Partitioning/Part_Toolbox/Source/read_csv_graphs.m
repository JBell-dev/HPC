% Script to load .csv lists of adjacency matrices and the corresponding 
% coordinates. 
% The resulting graphs should be visualized and saved in a .csv file.
%
% D.P & O.S for the "HPC Course" at USI and
%                   "HPC Lab for CSE" at ETH Zurich

addpaths_GP;
% i add the path that we need 
base_path = '/Users/jonatan/Documents/MAI/Y24/S1/HPC/TP6/Part_Toolbox';
csv_path = fullfile(base_path, 'Datasets/Countries_Meshes/csv');
output_path = fullfile(base_path, 'Datasets/Countries_Meshes/Countries_Mat');

countries = {'NO-9935', 'VN-4031'}; % Norway and Vietnam as asked 


% Steps
for i = 1:length(countries)
    country = countries{i};
    
    % 1. Load the .csv files
    adj_file = fullfile(csv_path, sprintf('%s-adj.csv', country)); %https://ch.mathworks.com/help/matlab/ref/string.sprintf.html
    pts_file = fullfile(csv_path, sprintf('%s-pts.csv', country));
    %help csvread -> it is suggested to not use this, so i made use of
    %readmatrix
    edges = readmatrix(adj_file); %we get the edges  -> out to in  (2 col)
    coords = readmatrix(pts_file); %we get the coords -> (x,y)
    
    % 2. Construct the adjaceny matrix (NxN). There are multiple ways
    %    to do so.
    n = size(coords, 1); %take the length of the coords
    %https://ch.mathworks.com/help/matlab/ref/sparse.html#d126e1665696
    adj_matrix = sparse(edges(:,1), edges(:,2), 1, n, n); %this stores "out to in"
    adj_matrix = adj_matrix + adj_matrix'; %now out to in is added by transpose - so now it is symmetric

    % 3. Visualize the resulting graphs
    figure('Name', sprintf('Graph of %s', country));
    gplotg(adj_matrix, coords);

    % 4. Save the resulting graphs
    saveas(gcf, fullfile(output_path, sprintf('%s_graph.png', country))); 
    save(fullfile(output_path, sprintf('%s_graph.mat', country)), 'adj_matrix', 'coords');
end



