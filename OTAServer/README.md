# OTAServer

## Production

## Development

### Requirements

- yarn installed globally
  - `npm i -g yarn`
- knex installed globally
  - `npm i -g knex`
- docker installed
  - `sudo apt install docker.io`
- docker-compose installed
  - `sudo apt install docker-compose`

### Setup instructions

#### Automatic setup

```bash
    yarn install
    && sudo docker-compose up -d
    && knex migrate:latest
    && yarn dev
```

#### Manual setup

1. Install dependencies

- `yarn install`

2. Start services

- `sudo docker-compose up -d`

3. Run migrations

- `knex migrate:latest`

4. Start dev server

- `yarn dev`

## docker-compose

The docker compose file configures following services

- postgres
- pgadmin

Please read the docker-compose file for more details.
