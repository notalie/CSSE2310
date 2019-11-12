int compare_resource_names(const void* s1, const void* s2) { 
	const Resource* firstResource = *((Resource**)s1);
	const Resource* secondResource = *((Resource**)s2);
    return strcmp(firstResource->name, secondResource->name); 
} 

int compare_neighbour_names(const void* s1, const void* s2) { 
	const Neighbour* firstNeighbour = *((Neighbour**)s1);
	const Neighbour* secondNeighbour = *((Neighbour**)s2);
    return strcmp(firstNeighbour->name, secondNeighbour->name); 
} 
