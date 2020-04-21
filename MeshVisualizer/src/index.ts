import cytoscape from "cytoscape";
import dagre from "cytoscape-dagre";
import popper from "cytoscape-popper";
import mqtt, { MqttClient } from "mqtt";

cytoscape.use(dagre);
cytoscape.use(popper);

const layout = {
  name: "dagre",
  animate: true,
  padding: 100,
  spacingFactor: 2,
  nodeSep: 120,
};

const cy = cytoscape({
  container: document.getElementById("app"),
  layout,
});

// cy.on("mouseover", "node", function (event) {
//   const node = event.target;
//   if (node.position().y > 0) {
//     const popper = node.popper({
//       content: () => {
//         let div = document.createElement("div");

//         div.style.fontSize = "40px";

//         div.innerHTML = "30 C";

//         document.body.appendChild(div);

//         return div;
//       },
//     });
//     node.one("mouseout", () => {
//       popper.destroy();
//     });
//   }
// });

let destroy;
setInterval(() => {
  if (destroy) destroy();
  destroy = displayPopup(2, Date.now().toString());
}, 2000);

function displayPopup(id: string | number, text: string) {
  const node = cy.getElementById(id);
  const popper = node.popper({
    content: () => {
      let div = document.createElement("div");

      div.style.fontSize = "40px";

      div.innerHTML = text;

      document.body.appendChild(div);

      return div;
    },
  });
  const destroy = () => popper.destroy();

  return destroy;
}

function getEdgesInPath(from: String, to: String) {
  const edges = cy.edges().map((ele) => ele.data());
  let edge = edges.find((e) => e.target === from);

  const result = [edge];

  while (edge.source !== to) {
    edge = edges.find((e) => e.target === edge.source);

    result.push(edge);
  }

  return result;
}

function highlightPath(from: String, to: String) {
  const edges = getEdgesInPath(from, to);

  for (const edge of edges) {
    cy.getElementById(edge.id).addClass("highlighted");
  }
}

function unhighlightPath(from: String, to: String) {
  const edges = getEdgesInPath(from, to);

  for (const edge of edges) {
    cy.getElementById(edge.id).removeClass("highlighted");
  }
}

function onNodeAdd(nodeInfo: any) {
  const parentNode = cy.getElementById(nodeInfo.parent);

  if (parentNode.length == 0) {
    cy.add([
      {
        data: {
          group: "nodes",
          id: nodeInfo.parent,
        },
        grabbable: false,
      },
      {
        data: {
          group: "nodes",
          id: nodeInfo.mac,
        },
        grabbable: false,
      },
      {
        data: {
          group: "edges",
          id: `${nodeInfo.parent},${nodeInfo.mac}`,
          source: nodeInfo.parent,
          target: nodeInfo.mac,
        },
      },
    ]);
  } else {
    cy.add([
      {
        data: {
          group: "nodes",
          id: nodeInfo.mac,
        },
      },
      {
        data: {
          group: "edges",
          id: `${nodeInfo.parent},${nodeInfo.mac}`,
          source: nodeInfo.parent,
          target: nodeInfo.mac,
        },
      },
    ]);
  }
  cy.layout(layout).run();
}

function onNodeRemove(nodeInfo: any) {
  cy.remove(cy.getElementById(nodeInfo.mac));
  cy.layout(layout).run();
}

function createMqttClient(url: string) {
  console.log("[MQTT]: Connecting to " + url);
  let mqttClient = mqtt.connect(url);

  mqttClient.subscribe("/node/#");

  mqttClient.on("message", (topic, _payload: Uint8Array) => {
    const payload = JSON.parse(_payload.toString());

    console.log("[MQTT]: Received " + topic);

    if (topic === "/node/add") onNodeAdd(payload);
    else if (topic === "/node/remove") onNodeRemove(payload);
    else {
      console.log(payload);
    }
  });

  return mqttClient;
}

let mqttClient: MqttClient;

document.getElementById("connectBtn").addEventListener("click", (_) => {
  if (mqttClient) {
    mqttClient.end();
    cy.remove(cy.nodes());
  }

  mqttClient = createMqttClient(
    (document.getElementById("mqttUrl") as HTMLInputElement).value
  );
});
