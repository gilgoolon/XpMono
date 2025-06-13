from glob import glob
import os
from pathlib import Path
import shutil
import uuid
import re
import argparse


FIG_TEMPLATE = "FileOperationsFig"

def generate_guid():
    return "{" + str(uuid.uuid4()).upper() + "}"

def update_projects(content, projects_map):
    lines = []
    guids = {}
    for line in content.split("\n"):
        if not line.startswith("Project(\""):
            lines.append(line)
            continue
        tokens = line.split(" = ", 1)
        fields = tokens[1].split(", ")
        project_name = fields[0].strip("\"")
        if project_name not in projects_map:
            lines.append(line)
            continue
        new_project_name = projects_map[project_name]
        guid = fields[2].strip("\"")
        generated_guid = generate_guid()
        new_line = tokens[0] + " = " + ", ".join([f"\"{new_project_name}\"", fields[1].replace(project_name, new_project_name), f"\"{generated_guid}\""])
        guids[guid] = generated_guid
        lines.append(new_line)
    new_content = "\n".join(lines)
    for key, value in guids.items():
        new_content = new_content.replace(key, value)
    return new_content

def duplicate_solution(source_folder, dest_folder, new_solution_name):
    if not os.path.exists(source_folder):
        print("Source folder does not exist.")
        return
    
    shutil.copytree(source_folder, dest_folder)
    
    old_solution_name = None
    for file in os.listdir(dest_folder):
        if file.endswith(".sln"):
            old_solution_name = file.strip(".sln")
            break
    
    if not old_solution_name:
        print("No solution file found.")
        return
    
    old_solution_path = dest_folder / f"{old_solution_name}.sln"
    new_solution_path = dest_folder / f"{new_solution_name}.sln"
    
    content = old_solution_path.read_text()
    
    project_names_to_update = {old_solution_name: new_solution_name, "Runner": "Runner"}
    content = update_projects(content, project_names_to_update)

    Path(new_solution_path).write_text(content)
    Path(old_solution_path).unlink()
    
    for key, value in project_names_to_update.items():
        if key == value:
            continue
        shutil.move(dest_folder / key, dest_folder / value)
        shutil.move(dest_folder / value / f"{key}.vcxproj", dest_folder / value / f"{value}.vcxproj")
        shutil.move(dest_folder / value / f"{key}.vcxproj.filters", dest_folder / value / f"{value}.vcxproj.filters")
        shutil.move(dest_folder / value / f"{key}.vcxproj.user", dest_folder / value / f"{value}.vcxproj.user")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Create a new Fig from an existing Fig template")
    parser.add_argument("name", type=str, help="New Fig solution name")
    args = parser.parse_args()
    
    repo_path = Path(os.getcwd()).parent
    template_folder = repo_path / FIG_TEMPLATE
    fig_name = args.name
    new_folder = repo_path / fig_name
    duplicate_solution(template_folder, new_folder, fig_name)
 