import { readFile } from 'fs/promises';
import ts from 'typescript';
export async function resolve(specifier, context, defaultResolve) {
  if (specifier.endsWith('.ts')) {
    return { url: new URL(specifier, context.parentURL).href, shortCircuit: true };
  }
  return defaultResolve(specifier, context, defaultResolve);
}
export async function load(url, context, defaultLoad) {
  if (url.endsWith('.ts')) {
    const source = await readFile(new URL(url));
    const result = ts.transpileModule(source.toString(), {
      compilerOptions: { module: ts.ModuleKind.ESNext, target: ts.ScriptTarget.ES2020 }
    });
    return { format: 'module', source: result.outputText, shortCircuit: true };
  }
  return defaultLoad(url, context, defaultLoad);
}
