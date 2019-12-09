import * as Knex from "knex";

export async function up(knex: Knex): Promise<any> {
  return knex.schema.alterTable("firmware", t => {
    t.unique(["name"]);
  });
}

export async function down(knex: Knex): Promise<any> {
  return knex.schema.alterTable("firmware", t => {
    t.dropUnique(["name"]);
  });
}
