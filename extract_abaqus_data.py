#!/usr/bin/env python3
"""
ABAQUS Input File Parser
Extracts data from simple.inp and creates separate files for the mechanobiology simulation
Handles multiple instances/parts and creates per-element material assignments
"""

import re
import os

def extract_nodes(input_file, output_file):
    """Extract node coordinates from all instances in ABAQUS input file"""
    with open(input_file, 'r') as f:
        content = f.read()
    
    all_nodes = []
    node_counter = 1
    
    # Find all instances
    instance_pattern = r'\*Instance, name=([^,]+).*?\n(.*?)\*End Instance'
    instances = re.findall(instance_pattern, content, re.DOTALL)
    
    for instance_name, instance_content in instances:
        # Find nodes in this instance
        node_pattern = r'\*Node\n(.*?)(?=\*Element|\*Nset|\*End)'
        node_match = re.search(node_pattern, instance_content, re.DOTALL)
        
        if node_match:
            node_data = node_match.group(1).strip()
            
            # Parse and renumber nodes sequentially
            for line in node_data.splitlines():
                line = line.strip()
                if line and not line.startswith('*'):
                    # Parse original node line
                    parts = line.replace(',', ' ').split()
                    if len(parts) >= 4:
                        # Renumber node and keep coordinates
                        new_line = f"{node_counter}, {parts[1]}, {parts[2]}, {parts[3]}"
                        all_nodes.append(new_line)
                        node_counter += 1
    
    # Write all nodes to file
    with open(output_file, 'w') as f:
        for node_line in all_nodes:
            f.write(node_line + '\n')
    
    print(f"Extracted {len(all_nodes)} nodes from all instances to {output_file}")
    return len(all_nodes)

def extract_elements(input_file, output_file):
    """Extract element connectivity from all instances, adjusting node references"""
    with open(input_file, 'r') as f:
        content = f.read()
    
    all_elements = []
    element_counter = 1
    node_offset = 0
    
    # Find all instances
    instance_pattern = r'\*Instance, name=([^,]+).*?\n(.*?)\*End Instance'
    instances = re.findall(instance_pattern, content, re.DOTALL)
    
    for instance_name, instance_content in instances:
        # Count nodes in this instance to determine offset
        node_pattern = r'\*Node\n(.*?)(?=\*Element|\*Nset|\*End)'
        node_match = re.search(node_pattern, instance_content, re.DOTALL)
        
        instance_node_count = 0
        if node_match:
            node_data = node_match.group(1).strip()
            for line in node_data.splitlines():
                line = line.strip()
                if line and not line.startswith('*'):
                    instance_node_count += 1
        
        # Find elements in this instance
        element_pattern = r'\*Element, type=C3D4\n(.*?)(?=\*Nset|\*Elset|\*End)'
        element_match = re.search(element_pattern, instance_content, re.DOTALL)
        
        if element_match:
            element_data = element_match.group(1).strip()
            
            # Parse elements and adjust node references
            for line in element_data.splitlines():
                line = line.strip()
                if line and not line.startswith('*'):
                    # Parse element line
                    parts = line.replace(',', ' ').split()
                    if len(parts) >= 5:  # element_id + 4 nodes
                        # Adjust node references by adding offset
                        nodes = [int(parts[i]) + node_offset for i in range(1, 5)]
                        new_line = f"{element_counter}, {nodes[0]}, {nodes[1]}, {nodes[2]}, {nodes[3]}"
                        all_elements.append((new_line, instance_name))
                        element_counter += 1
        
        # Update offset for next instance
        node_offset += instance_node_count
    
    # Write all elements to file
    with open(output_file, 'w') as f:
        for element_line, _ in all_elements:
            f.write(element_line + '\n')
    
    print(f"Extracted {len(all_elements)} elements from all instances to {output_file}")
    return all_elements

def extract_materials(input_file, output_file, elements_with_instances):
    """Create material assignment for each element based on section assignments"""
    with open(input_file, 'r') as f:
        content = f.read()
    
    # Extract material properties
    material_props = {}
    material_pattern = r'\*Material, name=([^,\n]+)\n\*Elastic\n([\d\.,\s]+)'
    material_matches = re.findall(material_pattern, content)
    
    for mat_name, elastic_props in material_matches:
        mat_name = mat_name.strip()
        props = elastic_props.strip().replace(' ', '').split(',')
        if len(props) >= 2:
            young_modulus = float(props[0])
            poisson_ratio = float(props[1])
            material_props[mat_name] = (young_modulus, poisson_ratio)
    
    # Extract section assignments per instance
    instance_materials = {}
    instance_pattern = r'\*Instance, name=([^,]+).*?\n(.*?)\*End Instance'
    instances = re.findall(instance_pattern, content, re.DOTALL)
    
    for instance_name, instance_content in instances:
        # Find section assignment in this instance
        section_pattern = r'\*Solid Section.*?material=([^,\n]+)'
        section_match = re.search(section_pattern, instance_content)
        
        if section_match:
            material_name = section_match.group(1).strip()
            instance_materials[instance_name] = material_name
    
    # Create material file with per-element assignments
    with open(output_file, 'w') as f:
        for i, (_, instance_name) in enumerate(elements_with_instances, 1):
            material_name = instance_materials.get(instance_name, "Material-1")  # default
            
            if material_name in material_props:
                young, poisson = material_props[material_name]
            else:
                # Default values if material not found
                young, poisson = 0.2, 0.167
            
            f.write(f"{i}, {young}, {poisson}\n")
    
    print(f"Created material assignments for {len(elements_with_instances)} elements to {output_file}")

def extract_model_template(input_file, output_file, elements_with_instances):
    """Extract and modify the model template for per-element materials"""
    with open(input_file, 'r') as f:
        content = f.read()
    
    # Parse the content and reconstruct with proper section assignments
    lines = content.splitlines()
    result_lines = []
    
    current_element_global = 1  # Track global element numbers
    inside_instance = False
    instance_element_count = 0
    
    i = 0
    while i < len(lines):
        line = lines[i]
        
        # Check if we're starting an instance
        if line.startswith('*Instance'):
            inside_instance = True
            instance_element_count = 0
            result_lines.append(line)
        
        # Check if we're ending an instance - add section assignments here
        elif line.startswith('*End Instance'):
            # Add individual element sets and section assignments for this instance
            if instance_element_count > 0:
                for elem_num in range(instance_element_count):
                    global_elem_num = current_element_global + elem_num
                    local_elem_num = elem_num + 1
                    
                    result_lines.append(f"*Elset, elset=Set-{global_elem_num}")
                    result_lines.append(f"{local_elem_num}")
                    result_lines.append(f"** Section: Section-{global_elem_num}")
                    result_lines.append(f"*Solid Section, elset=Set-{global_elem_num}, material=MAT_{global_elem_num}")
                    result_lines.append(",")
                
                current_element_global += instance_element_count
            
            result_lines.append(line)
            inside_instance = False
        
        # Count elements in this instance
        elif inside_instance and line.startswith('*Element'):
            result_lines.append(line)
            # Count following element lines
            j = i + 1
            while j < len(lines) and not lines[j].startswith('*') and lines[j].strip():
                instance_element_count += 1
                j += 1
        
        # Skip old section assignments
        elif (line.startswith('** Section:') or 
              line.startswith('*Solid Section') or 
              (line.strip() == ',' and i > 0 and lines[i-1].startswith('*Solid Section'))):
            pass  # Skip these lines
        
        # Skip material definitions (Update_model.cpp will add these)
        elif line.startswith('** MATERIALS') or line.startswith('*Material, name='):
            break  # Stop processing here
        
        else:
            result_lines.append(line)
        
        i += 1
    
    # Write the reconstructed template
    with open(output_file, 'w') as f:
        f.write('\n'.join(result_lines))
        f.write('\n')
    
    print(f"Extracted and modified model template to {output_file}")
    print(f"Added section assignments for {len(elements_with_instances)} elements within instances")
    return len(elements_with_instances)

def main():
    input_file = "data/simple_rat_600.inp"
    
    if not os.path.exists(input_file):
        print(f"Error: {input_file} not found")
        return
    
    print("Extracting data from all instances...")
    
    # Extract different components
    node_count = extract_nodes(input_file, "node_file.txt")
    elements_with_instances = extract_elements(input_file, "element_file.txt")
    extract_materials(input_file, "material_file.txt", elements_with_instances)
    extract_model_template(input_file, "Model.txt", elements_with_instances)
    
    print("Extraction complete!")
    print(f"Total nodes: {node_count}")
    print(f"Total elements: {len(elements_with_instances)}")

if __name__ == "__main__":
    main()