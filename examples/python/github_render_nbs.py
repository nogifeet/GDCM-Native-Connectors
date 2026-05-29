import nbformat

notebook_path = "./call_gdcm_converted.ipynb"
output_path = "./call_gdcm.ipynb"

MAX_OUTPUT_CHARS = 200_000   # reduce if GitHub still fails

with open(notebook_path, "r", encoding="utf-8") as f:
    nb = nbformat.read(f, as_version=4)

for cell in nb.cells:
    if cell.cell_type != "code":
        continue

    cleaned_outputs = []

    for output in cell.get("outputs", []):
        output_type = output.get("output_type")

        # Keep normal text outputs, but truncate very large ones
        if output_type == "stream":
            text = output.get("text", "")
            if isinstance(text, list):
                text = "".join(text)

            if len(text) > MAX_OUTPUT_CHARS:
                output["text"] = text[:MAX_OUTPUT_CHARS] + "\n\n...[truncated for GitHub rendering]..."
            cleaned_outputs.append(output)

        # Keep execute_result / display_data only if not huge HTML/base64
        elif output_type in ["execute_result", "display_data"]:
            data = output.get("data", {})

            # Remove problematic rich outputs
            data.pop("text/html", None)
            data.pop("application/javascript", None)
            data.pop("application/vnd.jupyter.widget-view+json", None)
            data.pop("application/vnd.plotly.v1+json", None)

            # Keep plain text
            if "text/plain" in data:
                text = data["text/plain"]
                if isinstance(text, list):
                    text = "".join(text)

                if len(text) > MAX_OUTPUT_CHARS:
                    data["text/plain"] = text[:MAX_OUTPUT_CHARS] + "\n\n...[truncated for GitHub rendering]..."

            # Keep PNG/JPEG images, but remove very large embedded images
            for img_key in ["image/png", "image/jpeg"]:
                if img_key in data:
                    img_data = data[img_key]
                    if isinstance(img_data, list):
                        img_data = "".join(img_data)

                    if len(img_data) > MAX_OUTPUT_CHARS:
                        data.pop(img_key, None)

            if data:
                output["data"] = data
                cleaned_outputs.append(output)

        # Keep errors, but truncate traceback if very long
        elif output_type == "error":
            traceback = output.get("traceback", [])
            joined = "\n".join(traceback)

            if len(joined) > MAX_OUTPUT_CHARS:
                output["traceback"] = [joined[:MAX_OUTPUT_CHARS] + "\n\n...[truncated for GitHub rendering]..."]

            cleaned_outputs.append(output)

    cell.outputs = cleaned_outputs

with open(output_path, "w", encoding="utf-8") as f:
    nbformat.write(nb, f)

print(f"Saved cleaned notebook with outputs: {output_path}")