const { spawnSync } = require('child_process');
const path = require('path');

function runWithEnv(env) {
  const result = spawnSync('node', [path.join(__dirname, '../index.js')], {
    env: { ...process.env, ...env },
    encoding: 'utf8'
  });
  return result;
}

test('fails when KEY_PATH is missing', () => {
  const { status, stderr } = runWithEnv({ KEY_PATH: undefined, CERT_PATH: 'cert', PORT: '3000' });
  expect(status).toBe(1);
  expect(stderr).toMatch(/Missing environment variable: KEY_PATH/);
});

test('fails when CERT_PATH is missing', () => {
  const { status, stderr } = runWithEnv({ KEY_PATH: 'key', CERT_PATH: undefined, PORT: '3000' });
  expect(status).toBe(1);
  expect(stderr).toMatch(/Missing environment variable: CERT_PATH/);
});

test('fails when PORT is missing', () => {
  const { status, stderr } = runWithEnv({ KEY_PATH: 'key', CERT_PATH: 'cert', PORT: undefined });
  expect(status).toBe(1);
  expect(stderr).toMatch(/Missing environment variable: PORT/);
});
