import React from "react";
import ReactDOM from "react-dom";
import { AppContainer } from "react-hot-loader";
import App from "./Index.bs";

const render = Component => {
  ReactDOM.render(
    React.createElement(AppContainer, null, React.createElement(App)),
    document.getElementById("main")
  );
};

render(App);

// Webpack Hot Module Replacement API
if (module.hot) {
  module.hot.accept("./Index.bs", () => {
    render(App);
  });
}
