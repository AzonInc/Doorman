import { readFileSync } from "fs";

function extractFunctionBody(src, signature)
{
  const start = src.indexOf(signature);
  if (start === -1)
  {
    throw new Error(`'${signature}' not found.`);
  }
 
  let depth = 0;
  let i = src.indexOf("{", start);
  const begin = i;
 
  while (i < src.length)
  {
    if (src[i] === "{")
    {
        depth++;
    }
    else if (src[i] === "}")
    {
      if (--depth === 0)
      {
        return src.slice(begin, i + 1);
      }
    }
    i++;
  }
  throw new Error(`End of '${signature}' not found.`);
}

export default {
  async load()
  {
    const src = readFileSync("../components/tc_bus_device/util.cpp", "utf8");

    // 1. model_mappings → enum → human name
    const modelNames = new Map();
    const arrayStart = src.indexOf("model_mappings[]");

    if (arrayStart !== -1)
    {
        const body = extractFunctionBody(src.slice(arrayStart), "{");
        const re = /\{(MODEL_\w+)\s*,\s*"([^"]+)"\s*\}/g;
        let m;
        while ((m = re.exec(body)) !== null) modelNames.set(m[1], m[2]);
    }
    
    // 2. getModelData → enum → { deviceGroup, caps }
    const body = extractFunctionBody(src, "ModelData getModelData(");
    
    const tokenRe = new RegExp(
        [
        /\bcase\s+(MODEL_\w+)\s*:/,
        /modelData\.device_group\s*=\s*(\d+)/,
        /\b(CAP_SETTING_\w+)\b/,
        /\bbreak\b/,
        ]
        .map((r) => r.source)
        .join("|"),
        "g"
    );
    
    const result = new Map();
    
    let currentEnums = [];
    let currentGroup = -1;
    let currentCaps  = [];
    const seenCaps   = new Set();
    
    function flush()
    {
        for (const e of currentEnums)
        {
            if (!result.has(e))
            {
                result.set(e, { deviceGroup: -1, caps: [] });
            }

            const entry = result.get(e);
            if (currentGroup !== -1)
            {
                entry.deviceGroup = currentGroup;
            }

            entry.caps.push(...currentCaps);
        }
    }
    
    let m;
    while ((m = tokenRe.exec(body)) !== null)
    {
        const [full, caseEnum, groupVal, cap] = m;
    
        if (caseEnum)
        {
            currentEnums.push(caseEnum);
        } 
        else if (groupVal !== undefined) 
        {
            currentGroup = parseInt(groupVal, 10);
        }
        else if (cap && !seenCaps.has(cap))
        {
            currentCaps.push(cap);
            seenCaps.add(cap);
        }
        else if (full === "break")
        {
            flush();
            currentEnums = [];
            currentGroup = -1;
            currentCaps  = [];
            seenCaps.clear();
        }
    }

    if (currentEnums.length)
    {
        flush();
    }
    
    return [...result.entries()]
    .filter(([enumName]) => !enumName.includes("DEBUG"))
    .sort(([enumA], [enumB]) => {
        const nameA = modelNames.get(enumA) ?? enumA;
        const nameB = modelNames.get(enumB) ?? enumB;
        return nameA.localeCompare(nameB);
    })
    .map(([enumName, { deviceGroup, caps }]) => ({
        enum:        enumName,
        name:        modelNames.get(enumName) ?? enumName,
        deviceGroup,
        caps:        caps.map((c) => c.replace(/^CAP_SETTING_/, "").toLowerCase()),
    }));
  },
};