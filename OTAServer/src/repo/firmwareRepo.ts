import knex, { queryToPromise } from "./knex";
import { Firmware } from "entity/Firmware";
import { omit } from "ramda";

export function findAll(): Promise<Firmware[]> {
  return queryToPromise(knex.select("*").from("firmware"));
}

export function findAllById(id: number): Promise<Firmware[]> {
  return queryToPromise(
    knex
      .select("*")
      .from<Firmware>("firmware")
      .where({
        id
      })
  );
}

export function findById(id: number): Promise<Firmware> {
  return queryToPromise(
    knex
      .select("*")
      .from<Firmware>("firmware")
      .where({
        id
      })
      .limit(1),
    true
  );
}

export function findByName(
  name: string,
  options?: {
    caseInsensitive: boolean;
  }
): Promise<Firmware> {
  if (options && options.caseInsensitive) {
    return queryToPromise(
      knex
        .select("*")
        .from<Firmware>("firmware")
        .whereRaw(`name ilike '${name}'`)
        .limit(1),
      true
    );
  }
  return queryToPromise(
    knex
      .select("*")
      .from<Firmware>("firmware")
      .where({
        name
      })
      .limit(1),
    true
  );
}

export function update(
  id: number,
  firmware: Firmware
): Promise<Firmware | null> {
  const changeset = omit(["id", "createdAt"], firmware);

  return queryToPromise(
    knex
      .update(changeset)
      .table("firmware")
      .where({ id })
      .returning("*"),
    true
  );
}

export function create(firmware: Firmware): Promise<Firmware> {
  const data = omit(["id"], firmware);

  return queryToPromise(
    knex
      .insert(data)
      .into("firmware")
      .returning("*"),
    true
  );
}
