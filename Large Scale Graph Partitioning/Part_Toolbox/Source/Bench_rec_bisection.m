% Benchmark for recursively partitioning meshes, based on various
% bisection approaches
%
% D.P & O.S for the "HPC Course" at USI and
%                   "HPC Lab for CSE" at ETH Zurich



% add necessary paths
addpaths_GP;
nlevels_a = 3;
nlevels_b = 4;

fprintf('       *********************************************\n')
fprintf('       ***  Recursive graph bisection benchmark  ***\n');
fprintf('       *********************************************\n')

% load cases
cases = {
    'airfoil1.mat';
    'netz4504_dual.mat';
    'stufe.mat';
    '3elt.mat';
    'barth4.mat';
    'ukerbe1.mat';
    'crack.mat';
    };

nc = length(cases);
maxlen = 0;
for c = 1:nc
    if length(cases{c}) > maxlen
        maxlen = length(cases{c});
    end
end

for c = 1:nc
    fprintf('.');
    sparse_matrices(c) = load(cases{c});
end


fprintf('\n\n Report Cases         Nodes     Edges\n');
fprintf(repmat('-', 1, 40));
fprintf('\n');
for c = 1:nc
    spacers  = repmat('.', 1, maxlen+3-length(cases{c}));
    [params] = Initialize_case(sparse_matrices(c));
    fprintf('%s %s %10d %10d\n', cases{c}, spacers,params.numberOfVertices,params.numberOfEdges);
end

%% Create results table
fprintf('\n%7s %16s %20s %16s %16s\n','Bisection','Spectral','Metis 5.0.2','Coordinate','Inertial');
fprintf('%10s %10d %6d %10d %6d %10d %6d %10d %6d\n','Partitions',8,16,8,16,8,16,8,16);
fprintf(repmat('-', 1, 100));
fprintf('\n');


for c = 1:nc
    spacers = repmat('.', 1, maxlen+3-length(cases{c}));
    fprintf('%s %s', cases{c}, spacers);
    sparse_matrix = load(cases{c});
    

    % Recursively bisect the loaded graphs in 8 and 16 subgraphs.
    % Steps
    % 1. Initialize the problem
    [params] = Initialize_case(sparse_matrices(c));
    W      = params.Adj;
    coords = params.coords;
    % 2. Recursive routines
    %the returns of the bisection are the map -> that  contain the
    %assignments to each partition of each vertex
    %-> sepij the list of separting edges between partitions
    %-> sepA adjacency matrix minus the separating edges.
    % i. Spectral    
    map_spec_8 = rec_bisection('bisection_spectral', nlevels_a, W, coords,0); %store the first only since we can pass that to the cutsize 
    map_spec_16 = rec_bisection('bisection_spectral', nlevels_b, W, coords,0);

    % ii. Metis
    map_metis_8 = rec_bisection('bisection_metis', nlevels_a, W, coords, 0);
    map_metis_16 = rec_bisection('bisection_metis', nlevels_b, W, coords, 0);

    % iii. Coordinate    
    map_coord_8 = rec_bisection('bisection_coordinate', nlevels_a, W, coords, 0);
    map_coord_16 = rec_bisection('bisection_coordinate', nlevels_b, W, coords, 0);

    % iv. Inertial
    map_inert_8 = rec_bisection('bisection_inertial', nlevels_a, W, coords, 0);
    map_inert_16 = rec_bisection('bisection_inertial', nlevels_b, W, coords, 0);

    % 3. Calculate number of cut edges
    cuts_spec_8 = cutsize(W, map_spec_8);
    cuts_spec_16 = cutsize(W, map_spec_16);
    cuts_metis_8 = cutsize(W, map_metis_8);
    cuts_metis_16 = cutsize(W, map_metis_16);
    cuts_coord_8 = cutsize(W, map_coord_8);
    cuts_coord_16 = cutsize(W, map_coord_16);
    cuts_inert_8 = cutsize(W, map_inert_8);
    cuts_inert_16 = cutsize(W, map_inert_16);

    % 4. Visualize the partitioning result
    if strcmp(cases{c}, 'crack.mat')
        % Spectral
        figure;
        gplotmap(W, coords, map_spec_16);
        title('Crack mesh - 16 partitions (Spectral)');
        colorbar;
        
        % Metis
        figure;
        gplotmap(W, coords, map_metis_16);
        title('Crack mesh - 16 partitions (Metis)');
        colorbar;
        
        % Coordinate
        figure;
        gplotmap(W, coords, map_coord_16);
        title('Crack mesh - 16 partitions (Coordinate)');
        colorbar;
        
        % Inertial
        figure;
        gplotmap(W, coords, map_inert_16);
        title('Crack mesh - 16 partitions (Inertial)');
        colorbar;
    end
    
    
    
    fprintf('%6d %6d %10d %6d %10d %6d %10d %6d\n', cuts_spec_8, cuts_spec_16, cuts_metis_8, cuts_metis_16, cuts_coord_8, cuts_coord_16, cuts_inert_8, cuts_inert_16);
    
end
