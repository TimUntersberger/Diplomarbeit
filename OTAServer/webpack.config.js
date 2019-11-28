const TsconfigPathsPlugin = require("tsconfig-paths-webpack-plugin");
const CleanTerminalPlugin = require("clean-terminal-webpack-plugin");

module.exports = {
  mode: "development",
  devtool: "inline-source-map",
  entry: "./src/index.ts",
  target: "node",
  node: false,
  output: {
    filename: "main.js"
  },
  plugins: [
    new CleanTerminalPlugin({
      message: "successfully compiled."
    })
  ],
  resolve: {
    extensions: [".ts", ".tsx", ".js"],
    plugins: [new TsconfigPathsPlugin()]
  },
  module: {
    rules: [
      { test: /\.tsx?$/, loader: "ts-loader", options: { transpileOnly: true } }
    ]
  }
};
