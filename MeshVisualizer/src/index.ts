import cytoscape from "cytoscape";
import dagre from "cytoscape-dagre";
import mqtt, { MqttClient } from "mqtt";

cytoscape.use(dagre);

const layout = {
  name: "dagre",
  animate: true,
  padding: 100,
  spacingFactor: 2,
  nodeSep: 120
};

const cy = cytoscape({
  container: document.getElementById("app"),
  style: [
    {
      selector: "node",
      style: {
        label: "data(id)",
        "background-color": "#45ba53"
      }
    }
  ],
  layout
});

function onNodeAdd(nodeInfo: any) {
  const parentNode = cy.getElementById(nodeInfo.parent);

  if (parentNode.length == 0) {
    cy.add([
      {
        data: {
          group: "nodes",
          id: nodeInfo.parent
        },
        grabbable: false
      },
      {
        data: {
          group: "nodes",
          id: nodeInfo.mac
        },
        grabbable: false
      },
      {
        data: {
          group: "edges",
          id: `${nodeInfo.parent},${nodeInfo.mac}`,
          source: nodeInfo.parent,
          target: nodeInfo.mac
        }
      }
    ]);
  } else {
    cy.add([
      {
        data: {
          group: "nodes",
          id: nodeInfo.mac
        }
      },
      {
        data: {
          group: "edges",
          id: `${nodeInfo.parent},${nodeInfo.mac}`,
          source: nodeInfo.parent,
          target: nodeInfo.mac
        }
      }
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
  });

  return mqttClient;
}

let mqttClient: MqttClient;

document.getElementById("connectBtn").addEventListener("click", _ => {
  if (mqttClient) {
    mqttClient.end();
    cy.remove(cy.nodes());
  }

  mqttClient = createMqttClient(
    (document.getElementById("mqttUrl") as HTMLInputElement).value
  );
});
